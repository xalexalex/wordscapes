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
 *
 * v2.0 with the Tree structure
 * 9 letters: 0.195s
 * 10 letters: 1.56s
 * 11 letters: 17.37s
 */

// TODO: alert if input or template > MAX_WORD_LEN

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YES 1
#define NO  0
#define MAX_WORD_LEN 35 /*this could safely be 10, IFF you ensure you discard words >10 chars properly */
#define DICT "/usr/share/dict/cracklib-small"

typedef struct tree {
	struct tree *p[27];
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
//FIXME both files can be substituted with data structures that make search faster. The first one is read once, while the second one is never created.

void add(Tree *root, char *word)
{
	//fprintf(stderr,"Attempting to add \"%s\" to found\n", word);

	char *p;
	int index;
	for(p = word; *p != '\0'; p++) {
		//fprintf(stderr,"in for, *p=%c\n", *p);
		index = *p - 'a';
		//fprintf(stderr,"in for, index=%d\n", index);
		if(index < 0 || index > 25)
			return; // unsupported character
		if(root->p[index] == NULL) {
			//fprintf(stderr,"adding leaf to found for character %c, index is %d...", *p, index);
			root->p[index] = new_Tree();
			//fprintf(stderr,"done\n");
		} else {
			//fprintf(stderr,"not adding because it's not null\n");
		}
		root = root->p[index];
	}

	if(*p == '\0')
		root->p[26] = new_Tree();
}

int is_in_tree(Tree *root, char *word)
{
	int index;
	for(int i = 0; i < strlen(word) + 1; i++, root = root->p[index]) {
		index = word[i]=='\0'? 26 : word[i]-'a';
		if(root->p[index] == NULL)
			return NO;
	}
	return YES;
}

Tree *new_Tree()
{
	Tree *ret = malloc(sizeof(Tree));
	if(ret == NULL)
		die("Couldn't allocate new_Tree");

	for(int i = 0; i < 27; i++)
		ret->p[i]=NULL;

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
	if(argc < 2 || argc > 3) {
		usage();
		return 1;
	}
	if(argc==3)
		template = argv[2];

	parse_dict();

	found = new_Tree();

	char *pre = malloc(sizeof(char) * (strlen(argv[1])+1));
	if(!pre)
		die("Couldn't allocate pre");
	memset(pre,0,sizeof(pre));

	perm(pre, argv[1]);

	return 0;
}
