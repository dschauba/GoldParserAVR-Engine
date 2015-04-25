
#ifndef _GPARSE_H_
#define _GPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// Symbol Types
//
#define SymbolTypeNonterminal		0
#define SymbolTypeTerminal			1
#define SymbolTypeWhitespace		2
#define SymbolTypeEnd End Character	3
#define SymbolTypeCommentStart		4
#define SymbolTypeCommentEnd		5
#define SymbolTypeCommentLine		6
#define SymbolTypeError				7

#define EEOF 512

//
// Callback function prototypes
//
typedef char (*_cbmatchtoken)(struct _parser* parser, void* user, short type, char* name, short symbol);
typedef void (*_cbneedinput)(struct _parse_input* pconfig);

typedef struct _symbol {
	char*	Name;
	short	Type;
} s_symbol;
typedef struct _rule {
	short	NonTerminal;
	short*	symbol;
	int		nsymbol;
} s_rule;
typedef struct _edge {
	short CharSetIndex;
	short TargetIndex;
} s_edge;
typedef struct _dfa_state {
	char Accept;
	short AcceptIndex;
	struct _edge*	edge;
	short	nedge;
} s_dfa_state;
typedef struct _action {
	short SymbolIndex;
	short Action;
	short Target;
} s_action;
typedef struct _lalr_state {
	struct _action*	action;
	short		naction;
} s_lalr_state;
typedef struct _token {
	short	id;
	char*	lexeme;
} s_token;
typedef struct _stack_element {
	struct _symbol	symbol;
	struct _token	token;
	short		state;
	void*		user_data;
} s_stack_element;
typedef struct _parser s_parser;
typedef struct _parse_input {
	struct _parser* parser;
	char*	buf;
	int		ncount;
	int		nofs;
	int		nbufsize;
	_cbneedinput cbneedinput;
	char	bpreserve;
	void*	user;
} s_parse_input;

// parser configuration
typedef struct _parse_config {
	short			init_dfa;
	short			init_lalr;
	char			case_sensitive;
	short			start_symbol;
	const char**	charset;
	short			ncharset;
	struct _dfa_state*	dfa_state;
	short			ndfa_state;
	struct _symbol*	sym;
	short			nsym;
	struct _rule*	rule;
	short			nrule;
	struct _lalr_state*	lalr_state;
	short			nlalr_state;
} s_parse_config;

// current parse info
typedef struct _parser {
	char					reduction;
	short					reduce_rule;
	char*					lexeme;
	int						nlexeme;
	short					symbol;
	short					lalr_state;
	void*					symbol_userdata;
	struct _stack_element*	stack;
	short					nstack;
	short					nstacksize;
	size_t					nstackofs;
	struct _parse_config*	pconfig;
	_cbmatchtoken			cbmatchtoken;
	void*					user_callback;
	struct _parse_input		input;
	struct _token*			tokens;
	short					ntokens;
} s_parser;

//
//	Parser functions
//
			// create from file
struct _parse_config*	parser_config_create_file(const char* filename);

			// create from memory
struct _parse_config*	parser_config_create_mem(char* buffer, int len);

			// delete
void		parser_config_delete(struct _parse_config* pconfig);


//
// Parser state functions
//

			// init the parser state
struct _parser*	parser_create(struct _parse_config* pconfig);

			// reset the parser state
void		parser_reset(struct _parser* parser);

			// delete the parser state
void		parser_delete(struct _parser* parser);

			// get a lexeme from the token stack
const char* parser_get_child_lexeme(struct _parser* parser, int index);
void*		parser_get_child_userdata(struct _parser* parser, int index);

			// set the current lexeme
void		parser_set_lexeme(struct _parser* parser, const char* lexeme, void* value);

			// get symbol info
char		parser_get_symbol(struct _parser* parser, int symbol, struct _symbol* psymbol);

			// get a pointer to the input structure
struct _parse_input* parser_get_input(struct _parser* parser);

			// get the last scanner lexeme
const char*	scanner_get_lexeme(struct _parser* parser);


//
// Scan/Parse functions
//
			// default scanner match function
char scanner_def_matchtoken(struct _parser* parser, void* user, short type, char* name, short symbol);

			// check for eof on the input stream
char scanner_get_eof(struct _parser* parser);

			// get the next character from the input stream
short scanner_get_char(struct _parser* parser);

			// increment to next character in input stream
void scanner_next_char(struct _parser* parser);

			// get the next token
short		scanner_scan(struct _parser* parser);

			// parse
short		parser_parse(struct _parser* parser);

#ifdef __cplusplus
}
//
// C++ Wrapper class
//
class Parser {
public:
	Parser(const char* config_filename) {
		m_config = 0; m_parser = 0;
		m_config = parser_config_create_file(config_filename);
		if (m_config) m_parser = parser_create(m_config);
	}
	~Parser() {
		if (m_parser) parser_delete(m_parser);
		if (m_config) parser_config_delete(m_config);
		m_parser = 0; m_config = 0;
	}
	char isopen() {return (m_config && m_parser)?1:0;}
	short parse() {return parser_parse(m_parser);}
	const char* get_child_lexeme(int idx) {return parser_get_child_lexeme(m_parser,idx);}
	void	set_lexeme(const char* lexeme, void* value) {parser_set_lexeme(m_parser,lexeme,value);}
	_parse_config*	m_config;
	_parser*		m_parser;
};
#endif

#endif
