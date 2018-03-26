/////////
// Genereate qgram from a document string. 
// This method will need to patch in th begining 
// and also in the end. 
// e.g.     abcd  ->  ##abcd$$
/////////



/* Change the defines of prefix patch and suffix patch */
/* The prefix patch is the one before the frst character  */
/* for example abc --> ##abc$$  ## is the prefix patch */
/* And the $$ is the suffix patch                     */


#define PREFIX_PATCH(x) (x - 1) /* Config the prefix length up to x - 1 */
#define SUFFIX_PATCH(x) (x - 1) /* Config the suffix length up to x - 1 */


/* define the number of qgrams after add prefix and suffix patches  */
#define DOC_TOK_NUM(x, y) (x - y + 1 + PREFIX_PATCH(y) + SUFFIX_PATCH(y)) 

#define DOC_TOK_START(x, q) (x - PREFIX_PATCH(q)) // Define the start pos
#define DOC_TOK_END(x, q) (DOC_TOK_START(x, q) + q) // Define the end pos

/* Define the Patch Characters */
#define PREFIX_PATCH_CHAR '#'
#define SUFFIX_PATCH_CHAR '$'


/* This algorithm get a string document and output a array     */
/* of qgrams after decompose it into patched version of qgrams */
/* return the number of qgrams generated                       */
int doc2QgramPatch (char **qglist, char *s, int slen, int q);


