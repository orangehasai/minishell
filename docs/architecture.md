# Minishell 実装ガイド — 全フェーズ詳細解説

---

## Phase 0: 共通基盤の設計（Week 1 前半 — 二人で実施）

このフェーズの目的は、二人が並行開発できるための「契約」を決めること。
ここで合意するデータ構造とインターフェースが、Phase 1 での分業の成否を決める。

### 0-1. プロジェクト構成

```
minishell/
├── Makefile
├── includes/
│   ├── minishell.h        # メイン共通ヘッダ
│   ├── lexer.h            # Person A
│   ├── parser.h           # Person A
│   ├── expander.h         # Person A
│   ├── executor.h         # Person B
│   ├── builtins.h         # Person B
│   └── signals.h          # 結合時
├── src/
│   ├── main.c
│   ├── lexer/
│   ├── parser/
│   ├── expander/
│   ├── executor/
│   ├── builtins/
│   ├── signals/
│   ├── env/               # 環境変数管理（共通）
│   └── utils/             # 共通ユーティリティ
└── libft/
```

### 0-2. 核となるデータ構造

ここが最も重要。二人が同じ構造体を前提にコードを書くため、最初に完全に合意する。

#### トークン（Lexer → Parser の受け渡し）

```c
typedef enum e_token_type
{
    TK_WORD,        // 通常の文字列（コマンド名、引数、ファイル名）
    TK_PIPE,        // |
    TK_REDIR_IN,    // <
    TK_REDIR_OUT,   // >
    TK_HEREDOC,     // <<
    TK_APPEND,      // >>
    TK_EOF          // 入力終端
}   t_token_type;

typedef struct s_token
{
    t_token_type    type;
    char            *value;     // トークン文字列（WORD の場合は中身、それ以外は NULL でも可）
    struct s_token  *next;
}   t_token;
```

**設計判断のポイント**: トークンの段階では `$VAR` の展開はまだ行わない。
`value` にはクォートを含んだ生の文字列がそのまま入る。
例えば `echo "hello $USER"` の場合、`"hello $USER"` という文字列がそのまま1つの `TK_WORD` になる。
展開とクォート除去は Expander フェーズで行う。

#### コマンドテーブル（Parser → Executor の受け渡し）

```c
typedef struct s_redir
{
    t_token_type    type;       // TK_REDIR_IN, TK_REDIR_OUT, TK_HEREDOC, TK_APPEND
    char            *file;      // リダイレクト先のファイル名（heredocの場合はデリミタ）
    struct s_redir  *next;
}   t_redir;

typedef struct s_cmd
{
    char            **argv;     // execve に渡す引数配列（argv[0] がコマンド名）
    t_redir         *redirs;    // このコマンドのリダイレクトリスト
    struct s_cmd    *next;      // パイプで繋がった次のコマンド（なければ NULL）
}   t_cmd;
```

**なぜ linked list か**: パイプラインは `cmd1 | cmd2 | cmd3` のように可変長。
linked list にすれば `next` を辿るだけで全コマンドを順に処理できる。
Executor は `cmd->next != NULL` ならパイプを作る、というシンプルな判定になる。

**なぜ redirs を別リストにするか**: 1つのコマンドに複数のリダイレクトが付く場合がある。
`< infile cmd > outfile 2>> errfile` のようなケース。
リストにしておけば、Executor は先頭から順に `open` + `dup2` していくだけでいい。

#### シェル全体の状態管理

```c
typedef struct s_shell
{
    t_env   *env;           // 環境変数 linked list
    int     last_status;    // 直前コマンドの終了ステータス（$? 用）
}   t_shell;
```

### 0-3. 環境変数の管理

`main` の第3引数 `char **envp` をそのまま使うと `export`/`unset` で困る。
起動時にコピーして自前の linked list で管理する。

```c
typedef struct s_env
{
    char            *key;       // "PATH" など
    char            *value;     // "/usr/bin:/bin" など（value がない場合は NULL）
    struct s_env    *next;
}   t_env;
```

必要な操作関数:

```c
int     env_init(t_env **env, char **envp); // envp → linked list に変換
char    *env_get(t_env *env, char *key); // キーから値を取得（なければ NULL）
int     env_set(t_env **env, char *key, char *value); // 追加/更新
int     env_unset(t_env **env, char *key);            // 削除
char    **env_to_array(t_env *env);      // execve 用に char** に変換
void    env_free(t_env *env);            // メモリ解放
```

**`env_to_array` が必要な理由**: `execve` の第3引数は `char **envp` を要求する。
linked list から毎回変換する。`"KEY=VALUE"` 形式の文字列配列を作る。

### 0-4. Makefile

```makefile
NAME        = minishell
CC          = cc
CFLAGS      = -Wall -Wextra -Werror
INCLUDES    = -I includes -I libft

# readline のパス（環境によって異なる）
# Mac (Homebrew): -I /opt/homebrew/opt/readline/include
# Linux: 不要な場合が多い
RL_INC      =
RL_LIB      = -lreadline

SRCS        = src/main.c \
              src/env/env_init.c \
              src/env/env_ops.c \
              # ... 各モジュールのファイルを追加

OBJS        = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	make -C libft
	$(CC) $(CFLAGS) $(OBJS) -L libft -lft $(RL_LIB) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(RL_INC) -c $< -o $@

clean:
	make -C libft clean
	rm -f $(OBJS)

fclean: clean
	make -C libft fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

### 0-5. main.c のスケルトン

```c
#include "minishell.h"

volatile sig_atomic_t   g_signal = 0;  // 唯一のグローバル変数

int main(int argc, char **argv, char **envp)
{
    t_shell shell;
    char    *line;

    (void)argc;
    (void)argv;
    shell.env = NULL;
    if (env_init(&shell.env, envp))
        return (1);
    shell.last_status = 0;
    while (1)
    {
        setup_signals_interactive();    // シグナル設定（Phase 2 で実装）
        line = readline("minishell$ ");
        if (!line)                      // ctrl-D → EOF
            break ;
        if (*line)
            add_history(line);
        // Phase 1 完成後:
        // tokens = lexer(line);
        // cmd = parser(tokens, &shell);
        // expand(cmd, &shell);
        // shell.last_status = execute(cmd, &shell);
        // free_tokens(tokens);
        // free_cmds(cmd);
        free(line);
    }
    env_free(shell.env);
    rl_clear_history();
    ft_putendl_fd("exit", STDOUT_FILENO);
    return (shell.last_status);
}
```

---

## Phase 1-A: パーシング（Person A — Week 1 後半〜Week 3）

### 1A-1. Lexer（トークナイザ）

入力文字列を `t_token` の linked list に変換する。これが Minishell で最も設計判断が多い部分。

#### 状態遷移モデル

Lexer は「現在どのモードにいるか」を追跡しながら1文字ずつ読み進める。

```
状態:
  STATE_NORMAL     — クォート外（メタ文字を解釈する）
  STATE_SQUOTE     — シングルクォート内（全てリテラル）
  STATE_DQUOTE     — ダブルクォート内（$ だけ特殊扱い、ただし展開はここではやらない）
```

#### 処理の流れ

```
入力: echo "hello $USER" | cat -e > out.txt

1. 空白をスキップ
2. 'e' → STATE_NORMAL → 英数字を読み続ける → "echo" → TK_WORD
3. 空白をスキップ
4. '"' → STATE_DQUOTE 開始 → '"' が来るまで読む → "hello $USER" → TK_WORD
   ※ この段階では $USER は展開しない。引用符も value に含めて保持する。
5. 空白をスキップ
6. '|' → TK_PIPE
7. 空白をスキップ
8. "cat" → TK_WORD
9. 空白をスキップ
10. "-e" → TK_WORD
11. 空白をスキップ
12. '>' → TK_REDIR_OUT（次の文字が '>' なら TK_APPEND）
13. 空白をスキップ
14. "out.txt" → TK_WORD
15. 文字列終端 → TK_EOF
```

#### 実装のコア部分

```c
t_token *lexer(char *line)
{
    t_token *head;
    t_token *tail;
    int     i;

    head = NULL;
    tail = NULL;
    i = 0;
    while (line[i])
    {
        skip_whitespace(line, &i);
        if (!line[i])
            break ;
        if (is_operator(line[i]))
            add_token(&head, &tail, read_operator(line, &i));
        else
            add_token(&head, &tail, read_word(line, &i));
    }
    add_token(&head, &tail, new_token(TK_EOF, NULL));
    return (head);
}
```

#### read_word の詳細（最も複雑な関数）

```c
// クォートの入れ子と切り替えを正しく処理する
// 例: echo he"ll"o → 1つの TK_WORD "he\"ll\"o" として読む（展開時に引用符除去）
// 例: echo 'it'"'"'s' → 1つの TK_WORD として読む

t_token *read_word(char *line, int *i)
{
    int     start;
    int     state;

    start = *i;
    state = STATE_NORMAL;
    while (line[*i])
    {
        if (state == STATE_NORMAL)
        {
            if (line[*i] == '\'')
                state = STATE_SQUOTE;
            else if (line[*i] == '"')
                state = STATE_DQUOTE;
            else if (is_whitespace(line[*i]) || is_operator(line[*i]))
                break ;
        }
        else if (state == STATE_SQUOTE && line[*i] == '\'')
            state = STATE_NORMAL;
        else if (state == STATE_DQUOTE && line[*i] == '"')
            state = STATE_NORMAL;
        (*i)++;
    }
    // 閉じられていないクォートはエラー（subject の要件: 解釈しない）
    if (state != STATE_NORMAL)
        return (NULL);  // エラーハンドリング
    return (new_token(TK_WORD, ft_substr(line, start, *i - start)));
}
```

**重要な注意点**: `he"ll"o` は1つの word。クォートの切れ目で word を分割してはいけない。
これは bash の仕様と同じ。隣接するクォート付き/なし部分は1つの word に結合される。

#### read_operator の詳細

```c
t_token *read_operator(char *line, int *i)
{
    if (line[*i] == '|')
        return ((*i)++, new_token(TK_PIPE, ft_strdup("|")));
    if (line[*i] == '<' && line[*i + 1] == '<')
        return ((*i) += 2, new_token(TK_HEREDOC, ft_strdup("<<")));
    if (line[*i] == '>' && line[*i + 1] == '>')
        return ((*i) += 2, new_token(TK_APPEND, ft_strdup(">>")));
    if (line[*i] == '<')
        return ((*i)++, new_token(TK_REDIR_IN, ft_strdup("<")));
    if (line[*i] == '>')
        return ((*i)++, new_token(TK_REDIR_OUT, ft_strdup(">")));
    return (NULL);
}
```

#### エラーケース

Lexer の段階で弾くべきエラー:

- 閉じられていないクォート → `bash: unexpected EOF while looking for matching quote`
- 特殊文字 `\` と `;` は subject で「解釈しない」とあるが、
  これは「エラーにする」ではなく「通常の文字として扱う」という意味。
  したがって、`\` や `;` が出てきたら TK_WORD の一部として読み進めればよい。

### 1A-2. Parser

トークン列を `t_cmd` の linked list に変換する。
Mandatory では `&&` `||` `()` はないため、文法は単純:

```
pipeline     : command ('|' command)*
command      : (redirection | WORD)+
redirection  : ('<' | '>' | '<<' | '>>') WORD
```

#### 実装

```c
t_cmd *parser(t_token *tokens)
{
    t_cmd   *head;
    t_cmd   *cmd;
    t_token *cur;

    head = NULL;
    cur = tokens;
    while (cur && cur->type != TK_EOF)
    {
        cmd = parse_command(&cur);
        if (!cmd)
            return (free_cmds(head), NULL);  // パースエラー
        append_cmd(&head, cmd);
        if (cur && cur->type == TK_PIPE)
            cur = cur->next;  // パイプを消費
    }
    return (head);
}
```

#### parse_command の詳細

```c
// 1つのコマンドをパースする
// 例: < infile grep "hello" > outfile
//   → argv = ["grep", "\"hello\""], redirs = [REDIR_IN "infile", REDIR_OUT "outfile"]

t_cmd *parse_command(t_token **cur)
{
    t_cmd   *cmd;
    int     argc;
    char    *argv_buf[1024];  // 仮バッファ（後で正確なサイズにコピー）

    cmd = ft_calloc(1, sizeof(t_cmd));
    argc = 0;
    while (*cur && (*cur)->type != TK_PIPE && (*cur)->type != TK_EOF)
    {
        if (is_redir_token((*cur)->type))
        {
            if (!parse_redir(cmd, cur))
                return (free_cmd(cmd), NULL);
        }
        else if ((*cur)->type == TK_WORD)
        {
            argv_buf[argc++] = (*cur)->value;
            *cur = (*cur)->next;
        }
        else
            return (syntax_error(*cur), free_cmd(cmd), NULL);
    }
    argv_buf[argc] = NULL;
    cmd->argv = copy_argv(argv_buf, argc);
    return (cmd);
}
```

#### パースエラーの検出

以下の場合にシンタックスエラーを出力する（bash と同様の挙動）:

```
|                        → パイプで始まっている
cmd |                    → パイプの後ろにコマンドがない
cmd | |                  → パイプが連続
> | < | >> | <<          → リダイレクトの後に WORD がない
> >                      → リダイレクトの後にリダイレクトが来ている
```

エラー出力: `minishell: syntax error near unexpected token 'XXX'`
エラー時の `$?` は `2`（bash と同じ）。

### 1A-3. Expander

パース済みの `t_cmd` の中の文字列に対して、変数展開とクォート除去を行う。

#### 処理順序（これが重要）

1. **変数展開** (`$VAR` → 値に置き換え)
2. **クォート除去** (引用符を取り除く)

この順序は bash の仕様に従う。変数展開はダブルクォート内でも行う。

#### 変数展開のルール

```
$VAR          → 環境変数 VAR の値。未定義なら空文字
$?            → 直前コマンドの終了ステータス
$             → そのまま $ として残る（$ の後に変数名がない場合）
'$VAR'        → シングルクォート内なので展開しない（文字列 "$VAR" のまま）
"$VAR"        → ダブルクォート内なので展開する
"$"           → 文字列 "$" のまま
```

#### 変数名の切れ目

`$` の後に続く変数名は、英数字とアンダースコアで構成される。
それ以外の文字が来たらそこで変数名が終わる。

```
$USER_NAME    → 変数 USER_NAME
$USER/bin     → 変数 USER + 文字列 "/bin"
$USER"hello"  → 変数 USER + 文字列 "hello"
${USER}       → subject にはないので対応不要（ただし bash なら対応する）
```

#### 展開の実装

```c
char *expand_str(char *str, t_shell *shell)
{
    char    *result;
    int     i;
    int     in_squote;
    int     in_dquote;

    result = ft_strdup("");
    i = 0;
    in_squote = 0;
    in_dquote = 0;
    while (str[i])
    {
        if (str[i] == '\'' && !in_dquote)
            in_squote = !in_squote;
        else if (str[i] == '"' && !in_squote)
            in_dquote = !in_dquote;
        else if (str[i] == '$' && !in_squote && str[i + 1]
                 && (ft_isalnum(str[i + 1]) || str[i + 1] == '_'
                     || str[i + 1] == '?'))
            result = expand_variable(result, str, &i, shell);
        else
            result = append_char(result, str[i]);
        if (str[i] && !(str[i] == '$' && !in_squote))  // $ 展開は内部で i を進める
            i++;
    }
    return (result);
}
```

#### クォート除去

展開後に引用符を除去する。この段階ではクォートの「保護」の役割は終わっている。

```c
char *remove_quotes(char *str)
{
    char    *result;
    int     i;
    int     in_squote;
    int     in_dquote;

    result = ft_strdup("");
    i = 0;
    in_squote = 0;
    in_dquote = 0;
    while (str[i])
    {
        if (str[i] == '\'' && !in_dquote)
            in_squote = !in_squote;
        else if (str[i] == '"' && !in_squote)
            in_dquote = !in_dquote;
        else
            result = append_char(result, str[i]);
        i++;
    }
    return (result);
}
```

#### Expander 全体の適用

```c
void expand(t_cmd *cmds, t_shell *shell)
{
    t_cmd   *cmd;
    t_redir *redir;
    int     i;
    char    *expanded;

    cmd = cmds;
    while (cmd)
    {
        i = 0;
        while (cmd->argv && cmd->argv[i])
        {
            expanded = expand_str(cmd->argv[i], shell);
            free(cmd->argv[i]);
            cmd->argv[i] = remove_quotes(expanded);
            free(expanded);
            i++;
        }
        redir = cmd->redirs;
        while (redir)
        {
            // heredoc デリミタにクォートがある場合は展開を無効にする
            // （Phase 2 の heredoc 実装時に対応）
            expanded = expand_str(redir->file, shell);
            free(redir->file);
            redir->file = remove_quotes(expanded);
            free(expanded);
            redir = redir->next;
        }
        cmd = cmd->next;
    }
}
```

### 1A-4. Person A のテスト戦略

Parser が正しく動いているかを確認するため、デバッグ用のプリント関数を用意する。

```c
void debug_print_cmds(t_cmd *cmd)
{
    int i;
    t_redir *r;

    while (cmd)
    {
        printf("CMD: ");
        i = 0;
        while (cmd->argv && cmd->argv[i])
            printf("[%s] ", cmd->argv[i++]);
        printf("\n");
        r = cmd->redirs;
        while (r)
        {
            printf("  REDIR: type=%d file=%s\n", r->type, r->file);
            r = r->next;
        }
        if (cmd->next)
            printf("  | (pipe)\n");
        cmd = cmd->next;
    }
}
```

テストケース（最低限これを通す）:

```bash
# 基本
echo hello world
# クォート
echo "hello world"
echo 'hello world'
echo "hello 'nested' world"
echo 'hello "nested" world'
echo he"ll"o
# 変数展開
echo $USER
echo "$USER"
echo '$USER'
echo "$USER is $HOME"
echo $?
# リダイレクト
< infile cat > outfile
cat << EOF
echo hello >> outfile
# パイプ
ls | grep .c | wc -l
# 複合
echo "hello $USER" | cat -e > out.txt
# エラー
|
echo |
> >
<<
```

---

## Phase 1-B: 実行エンジン（Person B — Week 1 後半〜Week 3）

### 1B-1. 基本的なコマンド実行

Person A のパーサーが完成する前でも、ハードコードした `t_cmd` で Executor を開発できる。

#### fork + execve の基本パターン

```c
int exec_simple_cmd(t_cmd *cmd, t_shell *shell)
{
    pid_t   pid;
    int     status;
    char    *path;

    if (is_builtin(cmd->argv[0]))
        return (exec_builtin(cmd, shell));
    pid = fork();
    if (pid < 0)
        return (perror("fork"), 1);
    if (pid == 0)
    {
        // 子プロセス
        apply_redirections(cmd->redirs);  // リダイレクト適用
        path = resolve_path(cmd->argv[0], shell);
        if (!path)
        {
            ft_putstr_fd("minishell: ", 2);
            ft_putstr_fd(cmd->argv[0], 2);
            ft_putendl_fd(": command not found", 2);
            exit(127);
        }
        execve(path, cmd->argv, env_to_array(shell->env));
        perror("execve");
        exit(126);
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        return (128 + WTERMSIG(status));
    return (1);
}
```

#### PATH の解決

```c
// コマンド名からフルパスを探す
// 1. "/" を含む場合 → 相対/絶対パスとしてそのまま使う
// 2. PATH から探す

char *resolve_path(char *cmd, t_shell *shell)
{
    char    *path_env;
    char    **dirs;
    char    *full_path;
    int     i;

    if (!cmd || !*cmd)
        return (NULL);
    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    path_env = env_get(shell->env, "PATH");
    if (!path_env)
        return (NULL);
    dirs = ft_split(path_env, ':');
    i = 0;
    while (dirs[i])
    {
        full_path = ft_strjoin3(dirs[i], "/", cmd);  // 3つ結合のユーティリティ
        if (access(full_path, X_OK) == 0)
            return (free_split(dirs), full_path);
        free(full_path);
        i++;
    }
    free_split(dirs);
    return (NULL);
}
```

**注意点**: `PATH` が未設定（`unset PATH` された場合）の場合、
PATH 検索を行わず、絶対/相対パスでのみコマンドを実行する。
bash はこの場合 `./cmd` やフルパスでの指定のみ受け付ける。

#### 終了ステータスの規則

```
正常終了:            WEXITSTATUS(status) → コマンドの exit code
シグナルで終了:       128 + WTERMSIG(status) → 例: SIGINT → 130, SIGQUIT → 131
コマンドが見つからない: 127
パーミッションエラー:  126
シンタックスエラー:    2
```

### 1B-2. Builtins（7つの組み込みコマンド）

Builtins は fork しないで（パイプ内を除く）親プロセスで直接実行する。
これは `cd` や `export` が親プロセスの状態を変更する必要があるため。

```c
int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    return (!ft_strcmp(cmd, "echo")
        || !ft_strcmp(cmd, "cd")
        || !ft_strcmp(cmd, "pwd")
        || !ft_strcmp(cmd, "export")
        || !ft_strcmp(cmd, "unset")
        || !ft_strcmp(cmd, "env")
        || !ft_strcmp(cmd, "exit"));
}

int exec_builtin(t_cmd *cmd, t_shell *shell)
{
    if (!ft_strcmp(cmd->argv[0], "echo"))
        return (builtin_echo(cmd));
    if (!ft_strcmp(cmd->argv[0], "cd"))
        return (builtin_cd(cmd, shell));
    if (!ft_strcmp(cmd->argv[0], "pwd"))
        return (builtin_pwd());
    if (!ft_strcmp(cmd->argv[0], "export"))
        return (builtin_export(cmd, shell));
    if (!ft_strcmp(cmd->argv[0], "unset"))
        return (builtin_unset(cmd, shell));
    if (!ft_strcmp(cmd->argv[0], "env"))
        return (builtin_env(shell));
    if (!ft_strcmp(cmd->argv[0], "exit"))
        return (builtin_exit(cmd, shell));
    return (1);
}
```

#### echo

```c
// echo [-n] [arg ...]
// -n: 末尾の改行を出力しない
// -nnnnn も有効（bash 互換: -n フラグの連続）
// -nab は無効（n 以外の文字が含まれる）

int builtin_echo(t_cmd *cmd)
{
    int i;
    int newline;

    i = 1;
    newline = 1;
    while (cmd->argv[i] && is_n_flag(cmd->argv[i]))
    {
        newline = 0;
        i++;
    }
    while (cmd->argv[i])
    {
        ft_putstr_fd(cmd->argv[i], STDOUT_FILENO);
        if (cmd->argv[i + 1])
            ft_putchar_fd(' ', STDOUT_FILENO);
        i++;
    }
    if (newline)
        ft_putchar_fd('\n', STDOUT_FILENO);
    return (0);
}

// -n フラグの判定: "-" の後に 'n' のみが1つ以上続く
int is_n_flag(char *arg)
{
    int i;

    if (!arg || arg[0] != '-' || arg[1] != 'n')
        return (0);
    i = 1;
    while (arg[i] == 'n')
        i++;
    return (arg[i] == '\0');
}
```

#### cd

```c
// cd [path]
// 引数なし → $HOME に移動
// "-" → $OLDPWD に移動（bonus 的だが bash 互換にするなら）
// 成功したら PWD と OLDPWD を更新

int builtin_cd(t_cmd *cmd, t_shell *shell)
{
    char    *path;
    char    *oldpwd;

    if (cmd->argv[1] && cmd->argv[2])
        return (ft_putendl_fd("minishell: cd: too many arguments", 2), 1);
    if (!cmd->argv[1])
    {
        path = env_get(shell->env, "HOME");
        if (!path)
            return (ft_putendl_fd("minishell: cd: HOME not set", 2), 1);
    }
    else
        path = cmd->argv[1];
    oldpwd = getcwd(NULL, 0);
    if (chdir(path) != 0)
    {
        ft_putstr_fd("minishell: cd: ", 2);
        perror(path);
        return (free(oldpwd), 1);
    }
    env_set(&shell->env, "OLDPWD", oldpwd);
    free(oldpwd);
    oldpwd = getcwd(NULL, 0);
    env_set(&shell->env, "PWD", oldpwd);
    free(oldpwd);
    return (0);
}
```

#### pwd

```c
int builtin_pwd(void)
{
    char *cwd;

    cwd = getcwd(NULL, 0);
    if (!cwd)
        return (perror("pwd"), 1);
    ft_putendl_fd(cwd, STDOUT_FILENO);
    free(cwd);
    return (0);
}
```

#### export

```c
// export [name[=value] ...]
// 引数なし → 全環境変数を "declare -x KEY="VALUE"" 形式で表示（ソート済み）
// 引数あり → 環境変数を設定/更新
// 無効な識別子はエラー（ただし残りは処理を続行）

int builtin_export(t_cmd *cmd, t_shell *shell)
{
    int     i;
    char    *key;
    char    *value;
    int     ret;

    if (!cmd->argv[1])
        return (print_export(shell->env));  // 全変数表示
    ret = 0;
    i = 1;
    while (cmd->argv[i])
    {
        if (!is_valid_identifier(cmd->argv[i]))
        {
            ft_putstr_fd("minishell: export: `", 2);
            ft_putstr_fd(cmd->argv[i], 2);
            ft_putendl_fd("': not a valid identifier", 2);
            ret = 1;
        }
        else
        {
            split_key_value(cmd->argv[i], &key, &value);
            env_set(&shell->env, key, value);
            free(key);
            // value は NULL の場合がある（"export FOO" のように = がない場合）
        }
        i++;
    }
    return (ret);
}

// 有効な識別子: 先頭が英字または _、以降は英数字または _
// = の前の部分だけを判定する
int is_valid_identifier(char *str)
{
    int i;

    if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
        return (0);
    i = 1;
    while (str[i] && str[i] != '=')
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}
```

#### unset

```c
// unset [name ...]
// 指定された環境変数を削除
// 存在しない変数を指定してもエラーにならない

int builtin_unset(t_cmd *cmd, t_shell *shell)
{
    int i;

    i = 1;
    while (cmd->argv[i])
    {
        env_unset(&shell->env, cmd->argv[i]);
        i++;
    }
    return (0);
}
```

#### env

```c
// env: 全環境変数を "KEY=VALUE" 形式で出力（value がないものは出力しない）

int builtin_env(t_shell *shell)
{
    t_env *cur;

    cur = shell->env;
    while (cur)
    {
        if (cur->value)  // value が NULL の変数は表示しない
        {
            ft_putstr_fd(cur->key, STDOUT_FILENO);
            ft_putchar_fd('=', STDOUT_FILENO);
            ft_putendl_fd(cur->value, STDOUT_FILENO);
        }
        cur = cur->next;
    }
    return (0);
}
```

#### exit

```c
// exit [n]
// 引数なし → last_status で終了
// 引数あり → その値で終了（数値でなければエラー）
// 引数が2つ以上 → エラー（ただし終了しない）

int builtin_exit(t_cmd *cmd, t_shell *shell)
{
    long long   n;

    ft_putendl_fd("exit", STDERR_FILENO);
    if (!cmd->argv[1])
        exit(shell->last_status);
    if (!is_numeric(cmd->argv[1]))
    {
        ft_putstr_fd("minishell: exit: ", 2);
        ft_putstr_fd(cmd->argv[1], 2);
        ft_putendl_fd(": numeric argument required", 2);
        exit(2);
    }
    if (cmd->argv[2])
    {
        ft_putendl_fd("minishell: exit: too many arguments", 2);
        return (1);  // 終了しない
    }
    n = ft_atoll(cmd->argv[1]);
    exit((unsigned char)n);  // 256 で mod を取る
}
```

### 1B-3. リダイレクション

コマンドのリダイレクトリストを順番に処理して、fd をすり替える。

```c
int apply_redirections(t_redir *redirs)
{
    int     fd;

    while (redirs)
    {
        if (redirs->type == TK_REDIR_IN)
            fd = open(redirs->file, O_RDONLY);
        else if (redirs->type == TK_REDIR_OUT)
            fd = open(redirs->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else if (redirs->type == TK_APPEND)
            fd = open(redirs->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else if (redirs->type == TK_HEREDOC)
        {
            fd = handle_heredoc(redirs->file);
            // heredoc は Phase 2 で詳細に実装
        }
        if (fd < 0)
        {
            ft_putstr_fd("minishell: ", 2);
            perror(redirs->file);
            return (-1);
        }
        if (redirs->type == TK_REDIR_IN || redirs->type == TK_HEREDOC)
            dup2(fd, STDIN_FILENO);
        else
            dup2(fd, STDOUT_FILENO);
        close(fd);
        redirs = redirs->next;
    }
    return (0);
}
```

**リダイレクトの適用順序が重要**: bash はリダイレクトを左から右に順番に処理する。

```bash
# 例: > out1 > out2 echo hello
# → out1 は作成されるが空、out2 に "hello" が書かれる
# → 最初に out1 を O_TRUNC で開き stdout に dup2
# → 次に out2 を O_TRUNC で開き stdout に dup2（out1 の fd は上書きされる）
```

### 1B-4. パイプ

パイプラインの実装は Minishell で最も間違いやすい部分。

```c
int exec_pipeline(t_cmd *cmds, t_shell *shell)
{
    int     pipe_fd[2];
    int     prev_fd;
    pid_t   pid;
    t_cmd   *cmd;
    int     status;
    int     last_pid;

    prev_fd = -1;
    cmd = cmds;
    while (cmd)
    {
        if (cmd->next)  // 次のコマンドがある → パイプを作る
        {
            if (pipe(pipe_fd) < 0)
                return (perror("pipe"), 1);
        }
        pid = fork();
        if (pid < 0)
            return (perror("fork"), 1);
        if (pid == 0)
        {
            // 子プロセス
            if (prev_fd != -1)              // 前のコマンドからの入力がある
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (cmd->next)                  // 次のコマンドへの出力がある
            {
                close(pipe_fd[0]);          // 読み取り側は閉じる
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }
            apply_redirections(cmd->redirs);
            if (is_builtin(cmd->argv[0]))
                exit(exec_builtin(cmd, shell));
            exec_child(cmd, shell);         // execve を呼ぶ（戻らない）
        }
        // 親プロセス
        if (prev_fd != -1)
            close(prev_fd);
        if (cmd->next)
        {
            close(pipe_fd[1]);              // 書き込み側は閉じる
            prev_fd = pipe_fd[0];           // 読み取り側を次のループへ
        }
        if (!cmd->next)
            last_pid = pid;
        cmd = cmd->next;
    }
    // 全子プロセスを待つ
    // last_pid のステータスだけが $? になる
    return (wait_all(last_pid));
}
```

**fd リークを防ぐための鉄則**:
1. 子プロセスでは使わない fd を必ず `close` する
2. 親プロセスでは `pipe_fd[1]` を fork 後に即座に `close` する
3. `prev_fd` は次のループの先頭で `close` する

**単一コマンドか複数コマンドかの分岐**:

```c
int execute(t_cmd *cmds, t_shell *shell)
{
    if (!cmds)
        return (0);
    if (!cmds->next && is_builtin(cmds->argv[0]))
    {
        // 単一 builtin → fork しないで親プロセスで実行
        // （ただしリダイレクションは一時的に適用して戻す）
        return (exec_builtin_with_redir(cmds, shell));
    }
    return (exec_pipeline(cmds, shell));
}
```

**単一 builtin のリダイレクト**: `cd > out` のようなケースでは、
親プロセスの stdout を一時的に退避して、dup2 でリダイレクトし、
builtin 実行後に元に戻す。

```c
int exec_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
    int saved_stdin;
    int saved_stdout;
    int ret;

    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
    if (apply_redirections(cmd->redirs) < 0)
    {
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdin);
        close(saved_stdout);
        return (1);
    }
    ret = exec_builtin(cmd, shell);
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
    return (ret);
}
```

### 1B-5. Person B のテスト用ハーネス

Parser がまだできていなくても、ハードコードした `t_cmd` でテストする。

```c
// テスト用: "ls -la | grep .c > out.txt" 相当
void test_executor(t_shell *shell)
{
    t_cmd   cmd2 = {0};
    t_redir redir = {TK_REDIR_OUT, "out.txt", NULL};
    char    *argv2[] = {"grep", ".c", NULL};
    cmd2.argv = argv2;
    cmd2.redirs = &redir;

    t_cmd   cmd1 = {0};
    char    *argv1[] = {"ls", "-la", NULL};
    cmd1.argv = argv1;
    cmd1.next = &cmd2;

    shell->last_status = execute(&cmd1, shell);
}
```

---

## Phase 2: 結合とシグナル（Week 3 後半〜Week 4 — 二人で実施）

### 2-1. Parser + Executor の結合

main.c のループを完成させる:

```c
while (1)
{
    setup_signals_interactive();
    line = readline("minishell$ ");
    if (!line)
        break ;
    if (*line)
        add_history(line);
    tokens = lexer(line);
    free(line);
    if (!tokens)
        continue ;
    cmds = parser(tokens);
    if (!cmds)
    {
        shell.last_status = 2;
        free_tokens(tokens);
        continue ;
    }
    expand(cmds, &shell);
    shell.last_status = execute(cmds, &shell);
    free_tokens(tokens);
    free_cmds(cmds);
}
```

### 2-2. シグナルハンドリング

minishell のシグナル制御は3つの場面で異なる動作をする:

```
場面 1: プロンプト表示中（readline 待ち）
  SIGINT (ctrl-C)  → 新しいプロンプトを表示
  SIGQUIT (ctrl-\) → 無視
  EOF (ctrl-D)     → シェル終了

場面 2: 子プロセス実行中
  SIGINT (ctrl-C)  → 子プロセスに届く（親はそのまま待機）
  SIGQUIT (ctrl-\) → 子プロセスに届く

場面 3: heredoc 入力中
  SIGINT (ctrl-C)  → heredoc を中断してプロンプトに戻る
```

#### グローバル変数

```c
// minishell.h
extern volatile sig_atomic_t g_signal;
```

**`volatile sig_atomic_t` を使う理由**: シグナルハンドラは非同期に呼ばれるため、
コンパイラの最適化でメモリ読み書きが省略されないよう `volatile` が必要。
`sig_atomic_t` はシグナルハンドラ内で安全に読み書きできることが保証された型。

#### シグナルハンドラ

```c
void sigint_handler(int signum)
{
    g_signal = signum;
    write(1, "\n", 1);           // 改行を出力
    rl_on_new_line();            // readline に新しい行であることを通知
    rl_replace_line("", 0);     // 入力バッファをクリア
    rl_redisplay();              // プロンプトを再表示
}
```

#### 場面ごとの設定

```c
void setup_signals_interactive(void)
{
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = SIG_IGN;         // ctrl-\ は無視
    sigaction(SIGQUIT, &sa, NULL);
}

void setup_signals_child(void)
{
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;         // 子プロセスはデフォルト動作に戻す
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

void setup_signals_parent_wait(void)
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;         // 親は子を待っている間、自分は SIGINT を無視
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}
```

#### 実行フローへの組み込み

```c
// fork 前後でシグナル設定を切り替える
pid = fork();
if (pid == 0)
{
    setup_signals_child();    // 子プロセス: デフォルトに戻す
    // exec...
}
setup_signals_parent_wait();  // 親: 子を待っている間は無視
waitpid(pid, &status, 0);
setup_signals_interactive();  // 待機終了後: インタラクティブモードに戻す

// 子がシグナルで終了した場合のメッセージ
if (WIFSIGNALED(status))
{
    if (WTERMSIG(status) == SIGQUIT)
        ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
    else if (WTERMSIG(status) == SIGINT)
        ft_putchar_fd('\n', STDOUT_FILENO);
}
```

#### g_signal の活用

readline() から戻った後、`g_signal` をチェックして `$?` を更新する:

```c
line = readline("minishell$ ");
if (g_signal == SIGINT)
{
    shell.last_status = 130;  // bash 互換: SIGINT → 130
    g_signal = 0;
}
```

### 2-3. Heredoc

heredoc は特殊で、パース時ではなく実行前に入力を読み取る必要がある。

```c
int handle_heredoc(char *delimiter)
{
    int     pipe_fd[2];
    char    *line;

    if (pipe(pipe_fd) < 0)
        return (-1);
    while (1)
    {
        line = readline("> ");
        if (!line || !ft_strcmp(line, delimiter))
        {
            free(line);
            break ;
        }
        // デリミタにクォートがなければ変数展開する
        // （Expander との連携が必要）
        ft_putendl_fd(line, pipe_fd[1]);
        free(line);
    }
    close(pipe_fd[1]);
    return (pipe_fd[0]);  // 読み取り側の fd を返す
}
```

**heredoc + シグナルの注意点**: heredoc 入力中に ctrl-C が押された場合、
heredoc を中断してプロンプトに戻る必要がある。
これは `g_signal` をチェックするか、readline のコールバックを使うか、
あるいは heredoc を子プロセスで実行して SIGINT で終了させるアプローチがある。

子プロセスアプローチが最もクリーン:

```c
int handle_heredoc_safe(char *delimiter, t_shell *shell)
{
    int     pipe_fd[2];
    pid_t   pid;
    int     status;

    pipe(pipe_fd);
    pid = fork();
    if (pid == 0)
    {
        close(pipe_fd[0]);
        setup_signals_heredoc();  // SIGINT でプロセス終了する設定
        heredoc_child_loop(delimiter, pipe_fd[1], shell);
        close(pipe_fd[1]);
        exit(0);
    }
    close(pipe_fd[1]);
    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status) || (WIFEXITED(status) && WEXITSTATUS(status) == 130))
    {
        close(pipe_fd[0]);
        return (-1);  // ctrl-C で中断された
    }
    return (pipe_fd[0]);
}
```

### 2-4. 結合時のデバッグチェックリスト

1. `echo $?` が直前コマンドの終了ステータスを正しく返すか
2. `export` → 新しいシェルで `env` → 変数が反映されているか
3. `cd /tmp && pwd` が `/tmp` を表示するか（&& は bonus だが、`cd /tmp` 後に `pwd`）
4. `cat | cat | ls` で ctrl-C → 全プロセスが終了し新しいプロンプトが出るか
5. `cat << EOF` で入力し、ctrl-C → プロンプトに戻るか
6. `"" | ""` のような空コマンドで segfault しないか
7. 存在しないコマンド → `$?` が 127 か
8. `chmod 000 file && ./file` → `$?` が 126 か

---

## Phase 3: テスト・仕上げ（Week 4 後半〜Week 5 — 二人で実施）

### 3-1. メモリリーク対策

```bash
# valgrind で自前コードのリークを確認
# readline 由来のリークは無視して良い（subject に明記）
valgrind --leak-check=full --show-leak-kinds=all \
         --suppressions=readline.supp ./minishell

# readline.supp の例:
# {
#    readline_leak
#    Memcheck:Leak
#    ...
#    fun:readline
# }
```

**よくあるリークパターン**:
- `ft_split` の戻り値を解放し忘れ
- `ft_strjoin` の戻り値を解放し忘れ（中間文字列）
- `env_to_array` の戻り値を解放し忘れ
- `getcwd(NULL, 0)` の戻り値を解放し忘れ
- `readline` の戻り値を解放し忘れ
- エラーパス（early return）での解放漏れ

#### 解放の原則

```c
// パターン: 中間文字列を free する
char *old = result;
result = ft_strjoin(result, new_part);
free(old);  // 忘れがち

// パターン: エラー時のクリーンアップ
t_token *tokens = lexer(line);
if (!tokens)
    return (free(line), continue);  // line を free してから continue
```

### 3-2. エッジケーステスト

```bash
# 空入力
<Enter>

# 空白のみ
   

# クォートのエッジケース
echo ""
echo ''
echo "'hello'"
echo '"hello"'
echo "hello""world"

# 変数展開のエッジケース
echo $
echo $""
echo "$"
echo $=
echo $123     # 数字で始まる → $ + "123"
echo $$       # PID（未実装でOK、$ が2つ残る）

# リダイレクトのエッジケース
> outfile     # コマンドなし → outfile を作成するだけ
< /dev/null cat
cat < nonexistent  # エラー

# パイプのエッジケース
ls | | grep    # syntax error
| ls           # syntax error
ls |           # syntax error

# Builtin のエッジケース
cd             # HOME に移動
cd ~           # ~ の展開は subject にないので文字通り "~" ディレクトリを探す
export A       # = なし → 変数は作られるが値なし
export =bad    # invalid identifier
exit 42
exit abc       # numeric argument required
exit 1 2       # too many arguments（終了しない）
exit 9999999999999999999999  # overflow

# 長いパイプライン
yes | head -n 5 | cat -e | wc -l

# heredoc
cat << EOF
hello
world
EOF
```

### 3-3. Norminette 対策

42 の Norm で引っかかりやすいポイント:
- 1関数25行以内 → 関数を細かく分割する
- 1ファイル5関数以内
- `for` ループ禁止 → `while` を使う
- 変数宣言は関数の先頭で → 途中で宣言しない
- 行は80文字以内
- グローバル変数は1個まで（`g_signal` のみ）
- 構造体は `typedef` 必須ではないが、42 Norm では `typedef` 推奨

### 3-4. README.md

subject の要件に沿って:

```markdown
*This project has been created as part of the 42 curriculum by <login1>, <login2>.*

## Description

Minishell is a simplified Unix shell implementation written in C.
It replicates core Bash functionality including command execution,
pipes, redirections, environment variable expansion, and built-in commands.

## Instructions

### Compilation

    make

### Execution

    ./minishell

### Cleaning

    make clean    # Remove object files
    make fclean   # Remove object files and executable
    make re       # Rebuild

## Features

- Interactive prompt with command history
- Command execution via PATH resolution or absolute/relative paths
- Built-in commands: echo, cd, pwd, export, unset, env, exit
- Pipes and redirections (<, >, <<, >>)
- Environment variable expansion ($VAR, $?)
- Single and double quote handling
- Signal handling (ctrl-C, ctrl-D, ctrl-\)

## Resources

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/)
- [The Open Group Base Specifications - Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html)
- [Writing Your Own Shell](https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf)

### AI Usage

AI tools were used to:
- Clarify Bash behavior for edge cases (quoting rules, signal handling)
- Debug specific memory management issues
- Generate initial test cases

All AI-generated content was reviewed, tested, and adapted by the team.
```

---

## 分業インターフェースまとめ

Person A と Person B の間の「契約」は以下の3つ:

1. **`t_token` 構造体**: Lexer の出力形式。これは Phase 0 で確定。
2. **`t_cmd` 構造体**: Parser の出力 / Executor の入力。これも Phase 0 で確定。
3. **`t_shell` 構造体**: 共有状態（環境変数、終了ステータス）。

この3つが決まっていれば、Person A は Executor を知らなくても Parser を書けるし、
Person B は Parser を知らなくても Executor を書ける。
結合時に初めて「A の出力を B の入力に繋ぐ」作業をする。
