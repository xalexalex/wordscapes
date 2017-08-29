/*
 * This small program finds all anagrams of a set of letters that are real words.
 * Optionally, when a template such as h_llo or ___y is given, only words matching it are printed.
 */

/* benchmark
 * letters	b94908b		239bba2		-O2
 * 5		1.2s		
 * 6		8.2s		
 * 7		58s		
 * 8		7m 55s		
 * 9		~ 1h 4m		0.195s
 * 10		~ 8h 32m	1.56s		0.7s
 * 11		~ 68h 16m	17.4s		7.6s
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YES 1
#define NO  0
#define MAX_WORD_LEN 35 /*this could safely be 10, IFF you ensure you discard words >10 chars properly */
// TODO: alert if input or template > MAX_WORD_LEN
#define DICT "/usr/share/dict/cracklib-small"

typedef struct tree {
	struct tree *p[26]; // a-z
	int last;
} Tree;

void add(Tree *root, char *word);
int is_in_tree(Tree *root, char *word);
Tree *new_Tree();
void usage();
void die(char *e);
void check(char *str);
void perm(char *pre, char *str);
void parse_dict();

char *argv0;
char *template = NULL;
Tree *dict, *found;

void add(Tree *root, char *word)
{
	char *p = word;
	Tree *t = root;
	int index;

	for( ; *p != '\0'; p++, t = t->p[index]) {
		index = *p-'a';
		if(index < 0 || index > 25)
			return; // unsupported character
		if(t->p[index] == NULL)
			t->p[index] = new_Tree();
	}
	t->last = YES;
}

int is_in_tree(Tree *root, char *word)
{
	int index;
	char *p = word;
	Tree *t = root;
	for( ; *p != '\0'; p++, t = t->p[index]) {
		index = *p - 'a';
		if(t->p[index] == NULL)
			return NO;
	}
	return t->last == YES ? YES : NO;
}

Tree *new_Tree()
{
	Tree *ret = malloc(sizeof(Tree));

	if(ret == NULL)
		die("Couldn't allocate new_Tree");

	memset(ret,0,sizeof(*ret));
	// ret->last=NO;

	return ret;
}

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

void die(char *e)
{
	fputs(e, stderr);
	exit(1);
}

void check(char *str)
{
	if(is_in_tree(dict,str)==YES && is_in_tree(found,str)==NO) {
		puts(str);
		add(found,str);
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

void parse_dict()
{
	FILE *dictfile;
	dict = new_Tree();

	dictfile = fopen(DICT, "r");
	if(!dictfile)
		die("Couldn't open "DICT);

	// FIXME can read char-by-char and add to dict directly
	char buf[MAX_WORD_LEN];
	while(fgets(buf, sizeof(buf), dictfile))
	{
		if(buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1]='\0';

		add(dict, buf);
	}

	fclose(dictfile);
}

int main(int argc, char *argv[])
{
	argv0=*argv;
	if(argc != 2 && argc != 3) {
		usage();
		return 1;
	}
	if(argc==3)
		template = argv[2];

	parse_dict();

	found = new_Tree();

	char *pre = calloc(strlen(argv[1])+1, sizeof(char));
	if(!pre)
		die("Couldn't allocate pre");

	perm(pre, argv[1]);

	return 0;
}
