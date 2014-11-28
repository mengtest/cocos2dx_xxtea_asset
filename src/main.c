//----------------------------------------------------
// linbc  2014.11.28
// http://linbc.cnblogs.com/
// github: https://github.com/linbc/cocos2dx_xxtea_asset

#include "xxtea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *progname = __FILE__; 

char    *optarg;                /* argument associated with option */
int getopt(int nargc,char * const nargv[],const char *ostr);

const char *_getprogname(void) {
	return progname;
}

static void usage(void) {
	printf("Usage:\n"
		"\n"
		"  %s [-e|-d] file1.lua -o file1.luac -s xxtea -k key -z \n"
		"\n"
		"Options:\n"
		"\n"
		"  -e [lua script|*.jpg]  encode file \n"
		"  -d <lua script|*.jpg>  decode file \n"
		"  -i input file \n"
		"  -o output file \n"
		"  -s set xxteaSign_string \n"
		"  -k set xxtea_key_string \n"
		"  -h                     Show this help message.\n"		
		"",
		progname);
	exit(1);
}

typedef struct opts_holder_t{
	int encode;
	int decode;
	int auto_by_header;

	char input[1024];
	char output[1024];
	char xxteasign[1024];
	char xxteakey[1024];
}opts_holder;

static void parse_opts(opts_holder *cf, int argc, char **argv) {
	int opt;
	int tmp;
	progname = argv[0];	

	memset(cf,0,sizeof(*cf));
	while (-1 != (opt = getopt(argc, argv, "i:o:s:k:aedh"))) {
		switch (opt) {
		case 'e':	cf->encode = 1;	break;
		case 'd':	cf->decode = 1;	break;
		case 'a':	cf->auto_by_header = 1;	break;

		case 'i':	strcpy(cf->input, optarg);	break;
		case 'o':	strcpy(cf->output, optarg);	break;
		case 's':	strcpy(cf->xxteasign, optarg);	break;
		case 'k':	strcpy(cf->xxteakey, optarg);	break;		

		default:
			usage();
		}
	}

	//解密及加密、根据文件头自动决定模式等不能同时存在
	tmp = cf->encode + cf->decode + cf->auto_by_header;
	if(tmp > 1 || tmp == 0)
		usage();

	//输入和输出都不可以为空
	if (strlen(cf->input) == 0 || strlen(cf->output) == 0)
		usage();
}

static int write_file(const char* sign, const char* content, int len, const char* file) {
	int sign_len = 0;	
	FILE *f = fopen(file,"wb");	
	if(!f) {
		printf("open file:%s failed!",file);
		return -1;
	}

	//当解码的时候就不需要文件头，入入空指针即可
	sign_len = sign == NULL? 0 :strlen(sign);
	if(sign_len > 0)
		fwrite(sign,1,sign_len,f);

	fwrite(content, 1, len, f);

	fclose(f);
	return 0;
}

static int read_file(const char* inputf,char **out) {
	char *p = NULL;	
	int total = 0;
	char temp[8096];

	FILE *f = fopen(inputf, "rb");
	if(!f) {
		printf("open file:%s failed!",inputf);
		return -1;
	}

	p = (char*)malloc(1);
	while(!feof(f)){
		int n = fread(temp,1,sizeof(temp),f);
		total += n;
		p = (char*)realloc(p,total + 1);
		memcpy(p + total - n,temp, n);
	}

	fclose(f);

	*out = p;
	return total;
}

static int encode_file(const char* input,const char*output, const char* sign,const char* key) {	
	char *in_content = NULL;
	int in_total = 0;
	char *encode_result = NULL;
	xxtea_long result_size = 0;

	//读取文件流
	if((in_total = read_file(input,&in_content)) == -1){
		return -1;
	}

	//加密
	encode_result = (char*)xxtea_encrypt((unsigned char*)in_content,in_total,
		(unsigned char*)key,strlen(key),&result_size);

	//写入文件流
	if(write_file(sign,encode_result,result_size,output)){
		free(encode_result);
		return -2;
	}

	//完工
	if (encode_result){
		free(encode_result);
		encode_result = NULL;
	}
	return 0;
}

static int decode_file(const char* input,const char*output, const char* sign,const char* key) {
	int sign_len = 0;
	char *in_content = NULL;
	int in_total = 0;
	char *encode_result = NULL;
	xxtea_long result_size = 0;

	//读取文件流
	if((in_total = read_file(input,&in_content)) == -1){
		return -1;
	}

	//验证一下文件头
	sign_len = strlen(sign);
	if(sign_len && strncmp(in_content, sign, sign_len) != 0) {
		return -2;
	}

	//解密, 忽略文件头
	encode_result = (char*)xxtea_decrypt((unsigned char*)in_content + sign_len,in_total - sign_len,
		(unsigned char*)key,strlen(key),&result_size);

	//写入文件流
	if(write_file(NULL,encode_result,result_size,output)){
		free(encode_result);
		return -3;
	}

	//完工
	if (encode_result){
		free(encode_result);
		encode_result = NULL;
	}
	return 0;
}

int main(int argc, char **argv){
	opts_holder opts;
	parse_opts(&opts,argc,argv);

	if (opts.encode){
		encode_file(opts.input, opts.output, opts.xxteasign, opts.xxteakey);
	} else if (opts.decode) {
		decode_file(opts.input, opts.output, opts.xxteasign, opts.xxteakey);
	}

	return 0;
}
