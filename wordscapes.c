/*
 * This small program finds all anagrams of a set of letters that are real words.
 * Optionally, when a template such as h_llo or ___y is given, only words matching it are printed.
 */

/* benchmarks
 * 5 letters: 1.2s
 * 6 letters: 8.2s
 * 7 letters: 58s
 * 8 letters: 7m 55s
 * 9 letters: ~ 1h 4m
 * 10 letters: ~ 8h 32m
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YES 1
#define NO  0
#define MAX_WORD_LEN 35
#define DICT "/usr/share/dict/cracklib-small"

char *argv0;
char *template = NULL;
FILE *dict;
FILE *found;
//FIXME both files can be substituted with data structures that make search faster. The first one is read once, while the second one is never created.

void usage()
{
	printf("Usage: %s word [template]\n\n"
			"Examples:\n"
			"%s pin\n"
			"pin\n"
			"nip\n\n"
			"%s pin n__\n"
			"nip\n", argv0, argv0, argv0);
}

int word_in_file(char *word, FILE *file)
{
	rewind(file);
	char buf[MAX_WORD_LEN];
	while(fgets(buf, sizeof(buf), file))
	{
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]='\0';
		if(strcmp(buf,word)==0) {
			return YES;
		}
	}
	return NO;
}

void check(char *str)
{
	if(word_in_file(str,dict)==YES && word_in_file(str,found)==NO) {
		puts(str);
		fseek(found,0,SEEK_END);
		fputs(str,found);
		fputc('\n',found);
	}
}

void perm(char *pre, char *str)
{
	if(template != NULL) {
		if(strlen(pre)>strlen(template))
			return;

		for(int i = 0; i < strlen(pre); i++) {
			if(template[i]=='_')
				continue;
			if(template[i]!=pre[i])
				return;
		}
	}

	int x=strlen(pre);

	if(x>2) {
		if(template == NULL || strlen(template) == strlen(pre))
			check(pre);
	}

	for(int i = 0; i < strlen(str); i++) {
		if(str[i]=='_')
			continue;

		pre[x]=str[i];
		pre[x+1]='\0';
		str[i]='_';
		perm(pre,str);
		str[i]=pre[x];
	}
}

int main(int argc, char *argv[])
{
	argv0=*argv;
	if(argc < 2 || argc > 3) {
		usage();
		return 1;
	}
	if(argc==3)
		template = argv[2];

	char *pre = malloc(sizeof(char) * (strlen(argv[1])+1));
	if(!pre) return 1;

	dict = fopen(DICT, "r");
	if(!dict) return 1;
	found = fopen("/tmp/found","w+");
	if(!found) return 1;

	perm(pre, argv[1]);

	return 0;
}
