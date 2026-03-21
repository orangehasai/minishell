# Minishell
---

## 目次

1. [開発ワークフロー](#開発ワークフロー)
2. [プロジェクト概要](#プロジェクト概要)
3. [ビルド・実行方法](#ビルド実行方法)
4. [機能一覧](#機能一覧)
5. [アーキテクチャ](#アーキテクチャ)
6. [データ構造の設計](#データ構造の設計)
7. [各フェーズの詳細](#各フェーズの詳細)
8. [分業とタスク管理](#分業とタスク管理)
9. [参考資料](#参考資料)

---

## 開発ワークフロー

### 全体の流れ

```
1. タスクを選ぶ ─── GitHub Issues から着手するタスクを選択
       │
2. ブランチを切る ─ git checkout -b feature/1a-3-lexer-word
       │
3. 実装する ─────── エディタで直接書く or Claude Code で書く or 併用
       │
4. ローカル確認 ─── make → ./minishell で動作テスト
       │
5. セルフレビュー ─ claude → /review（任意。Norm違反・リークを事前検出）
       │
6. push & PR作成 ── git push → GitHub で PR を作成
       │                  ┌──────────────────────────────┐
       └─── CI 自動実行 ──┤  norminette（Norm チェック）    │
                          │  make（ビルド確認）            │
                          │  valgrind（メモリリーク検出）   │
                          │  Claude Code（AI コードレビュー）│
                          └──────────────────────────────┘
       │
7. レビュー対応 ─── CI の指摘やチームメンバーのコメントを修正
       │
8. マージ ─────── 全チェック通過後にマージ
```

### Step 1〜2: タスク選択とブランチ作成

GitHub Issues に 33 タスクが登録されている。着手するタスクを選び、ブランチを切る。

```bash
# 手動で切る場合
git checkout -b feature/1a-3-lexer-word

# Claude Code を使う場合（タスク読み込み + ブランチ作成を自動でやる）
claude
> /implement 1a-3
```

### Step 3: 実装

エディタで直接書いても、Claude Code に任せても、併用してもいい。

```bash
# エディタで書く場合
vim src/lexer/lexer_word.c

# Claude Code で書く場合
claude
> read_word 関数を実装して。STATE_NORMAL/SQUOTE/DQUOTE の状態遷移で。

# 部分的に聞く場合
claude
> he"ll"o みたいな隣接クォートの結合ってどう実装すればいい？
```

### Step 4: ローカル確認

最低限 `make` が通ること、基本動作が正しいことを確認する。

```bash
make
./minishell
minishell$ echo "hello world"
minishell$ ls | grep .c
minishell$ exit
```

### Step 5: セルフレビュー（任意だが推奨）

PR を出す前に Claude Code でセルフレビューする。Norm 違反やリークを事前に潰せる。

```bash
claude
> /review          # 最新コミットの変更をレビュー
> /norm-fix        # Norm 違反を自動修正
> /leak-check      # valgrind でリーク検出（Mac では使えないので CI に任せても可）
```

### Step 6: push と PR 作成

```bash
git add -A
git commit -m "[lexer] クォート状態遷移の実装"
git push origin feature/1a-3-lexer-word
```

GitHub で PR を作成すると、**4つの CI ジョブが自動で走る**:

| ジョブ | 内容 | 失敗条件 |
|--------|------|----------|
| `build` | `make` でビルド + 不要な再リンク検出 | コンパイルエラー or 再リンク |
| `norminette` | 42 Norm v4.1 チェック | Norm 違反あり |
| `valgrind` | Ubuntu 上でメモリリーク検出 | 自前コードのリーク（readline 由来は除外） |
| `Claude Code Review` | AI が diff を読んで日本語でレビューコメント投稿 | — （コメントのみ、ブロックはしない） |

### Step 7: レビュー対応

CI の結果と Claude のレビューコメントを確認し、必要な修正をする。
PR のコメントで `@claude` と書くと追加の質問もできる。

```
@claude この関数のメモリリークの可能性を確認して
@claude builtin_cd が bash と違う挙動をするケースはある？
```

### Step 8: マージ

全 CI が通り、チームメンバーの Approve が得られたらマージ。

### ツール一覧

| ツール | 場所 | 用途 |
|--------|------|------|
| `CLAUDE.md` | リポジトリルート | Claude Code 起動時に自動読み込みされるプロジェクトコンテキスト |
| `/review` | `.claude/commands/` | Norm + メモリ + ロジックのコードレビュー |
| `/norm-fix` | `.claude/commands/` | Norm 違反の検出と自動修正 |
| `/implement [番号]` | `.claude/commands/` | タスク番号指定で実装開始 |
| `/leak-check` | `.claude/commands/` | valgrind でリーク検出 |
| `ci.yml` | `.github/workflows/` | ビルド + norminette + valgrind の自動チェック |
| `claude-review.yml` | `.github/workflows/` | Claude による PR 自動レビュー |
| `docs/architecture.md` | `docs/` | 設計詳細・実装ガイド |
| `docs/tasks.md` | `docs/` | 33 タスクの一覧と依存関係 |
| `docs/norm_v4.1.pdf` | `docs/` | 42 Norm 原文 |

---

## プロジェクト概要

Minishell は Bash の基本機能を再現したシェルを C 言語で実装するプロジェクトです。ユーザーの入力を受け取り、字句解析・構文解析・変数展開・実行の4段階で処理します。パイプ、リダイレクション、環境変数の展開、シグナル制御、7つの組み込みコマンドに対応しています。

```
readline() → Lexer → Parser → Expander → Executor → $?
     ↑                                                 |
     └─────────────── REPL ループ ─────────────────────┘
```

---

## ビルド・実行方法

### 前提条件

- `cc` コンパイラ
- `make`
- `readline` ライブラリ

macOS (Homebrew) の場合:

```bash
brew install readline
```

### コンパイル

```bash
make        # ビルド
make clean  # オブジェクトファイルを削除
make fclean # オブジェクトファイルと実行ファイルを削除
make re     # クリーンビルド
```

### 実行

```bash
./minishell
```

---

## 機能一覧

- `readline` によるインタラクティブなプロンプトとコマンド履歴
- PATH 検索または絶対/相対パスによるコマンド実行
- パイプ (`|`): コマンドの stdout を次のコマンドの stdin に接続
- リダイレクション: `<`（入力）、`>`（出力）、`>>`（追記）、`<<`（heredoc）
- 環境変数の展開（`$VAR`、`$?`）
- シングルクォートとダブルクォートの処理
- シグナル制御（`ctrl-C`、`ctrl-D`、`ctrl-\`）
- 組み込みコマンド: `echo`、`cd`、`pwd`、`export`、`unset`、`env`、`exit`

---

## アーキテクチャ

### 処理パイプラインの全体像

入力された1行は4つのステージを順に通過します。各ステージは入力と出力の型が明確に定義されており、モジュールを独立してテストできます。

```
┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐
│  Lexer   │───▶│  Parser  │───▶│ Expander │───▶│ Executor │
│          │    │          │    │          │    │          │
│ char*    │    │ t_token* │    │ t_cmd*   │    │ t_cmd*   │
│ → t_token*│   │ → t_cmd* │    │ → t_cmd* │    │ → int    │
└──────────┘    └──────────┘    └──────────┘    └──────────┘
```

| ステージ | 入力 | 出力 | 役割 |
|----------|------|------|------|
| Lexer | `char *line` | `t_token *` | 生の文字列をトークン列に分割（単語、パイプ、リダイレクション） |
| Parser | `t_token *` | `t_cmd *` | トークン列からコマンドテーブルを構築し、構文エラーを検出 |
| Expander | `t_cmd *` | `t_cmd *` | `$VAR` を値に置換し、クォートを除去 |
| Executor | `t_cmd *` | `int`（終了ステータス） | プロセスを fork し、パイプ/リダイレクションを設定してコマンドを実行 |

### 横断的な関心事

- **環境変数 (`t_env`)** — 起動時に `envp` からコピーした連結リスト。Expander が参照し、`export`/`unset`/`cd` が変更する。
- **シグナル** — プロンプト待ち、子プロセス実行中、heredoc 入力中の3モードで設定を切り替える。グローバル変数は `g_signal` の1個のみ。
- **シェルコンテキスト (`t_shell`)** — `env` と `last_status` を保持し、全関数にポインタで渡す。グローバル変数の代替。

### ディレクトリ構成

```
minishell/
├── Makefile
├── includes/
│   ├── minishell.h        # メイン共通ヘッダ
│   ├── lexer.h
│   ├── parser.h
│   ├── expander.h
│   ├── executor.h
│   ├── builtins.h
│   └── signals.h
├── src/
│   ├── main.c
│   ├── lexer/             # 字句解析
│   ├── parser/            # 構文解析
│   ├── expander/          # 変数展開・クォート除去
│   ├── executor/          # コマンド実行・パイプ・リダイレクション
│   ├── builtins/          # 組み込みコマンド
│   ├── signals/           # シグナル制御
│   ├── env/               # 環境変数管理
│   └── utils/             # 共通ユーティリティ
└── libft/
```

---

## データ構造の設計

### t_token — Lexer と Parser の間の共通言語

Lexer が生の入力文字列を意味のある単位に切り分けた結果。この段階ではクォートはそのまま保持され、`$VAR` の展開はまだ行わない。

```c
typedef enum e_token_type
{
    TK_WORD,        // コマンド名、引数、ファイル名
    TK_PIPE,        // |
    TK_REDIR_IN,    // <
    TK_REDIR_OUT,   // >
    TK_HEREDOC,     // <<
    TK_APPEND,      // >>
    TK_EOF          // 入力の終端
}   t_token_type;

typedef struct s_token
{
    t_token_type    type;
    char            *value;
    struct s_token  *next;
}   t_token;
```

**入力例:** `echo "hello $USER" | cat -e > out.txt`

```
[TK_WORD "echo"] → [TK_WORD "\"hello $USER\""] → [TK_PIPE "|"]
→ [TK_WORD "cat"] → [TK_WORD "-e"] → [TK_REDIR_OUT ">"]
→ [TK_WORD "out.txt"] → [TK_EOF]
```

**なぜ連結リストか:** トークンの数はパース前に分からない。連結リストなら出現順に追加するだけでよい。

### t_cmd / t_redir — Parser と Executor の間の共通言語

Parser がトークン列を `TK_PIPE` で分割し、各コマンドに必要な情報をまとめた構造体。Executor はこれだけを見て実行する。

```c
typedef struct s_redir
{
    t_token_type    type;   // TK_REDIR_IN, TK_REDIR_OUT, TK_HEREDOC, TK_APPEND
    char            *file;  // ファイル名（heredoc の場合はデリミタ）
    struct s_redir  *next;
}   t_redir;

typedef struct s_cmd
{
    char            **argv;     // execve に渡す NULL 終端配列
    t_redir         *redirs;    // このコマンドのリダイレクトリスト
    struct s_cmd    *next;      // パイプで繋がった次のコマンド（なければ NULL）
}   t_cmd;
```

**設計判断:**
- `argv` が `char **` なのは `execve()` がこの形式を要求するため。
- `next != NULL` は「パイプを作る」という意味。Executor はこの1フィールドで判断する。
- `redirs` が連結リストなのは、1つのコマンドに複数のリダイレクトが付く場合があり（`< in cmd > out`）、左から右の適用順序が意味を持つため。

**入力例:** 同じコマンドの場合

```
┌─── コマンド 1 ─────────────┐         ┌─── コマンド 2 ──────────────────────┐
│ argv = ["echo", "hello…"]  │────────▶│ argv = ["cat", "-e"]               │
│ redirs = NULL               │         │ redirs → [REDIR_OUT "out.txt"]     │
└────────────────────────────┘         │ next = NULL                         │
                                        └────────────────────────────────────┘
```

### t_env — 可変長の環境変数テーブル

起動時に `envp` からコピーした連結リスト。`export`/`unset` で動的に増減するため、固定長配列ではなく連結リストで管理する。

```c
typedef struct s_env
{
    char            *key;       // "PATH" など
    char            *value;     // "/usr/bin:/bin" など（値がない場合は NULL）
    struct s_env    *next;
}   t_env;
```

必要な操作関数:

| 関数 | 役割 |
|------|------|
| `env_init(envp)` | `char **envp` を連結リストに変換 |
| `env_get(env, key)` | キーから値を取得（なければ NULL） |
| `env_set(&env, key, value)` | 追加または更新 |
| `env_unset(&env, key)` | 削除 |
| `env_to_array(env)` | `execve` 用に `"KEY=VALUE"` 形式の `char **` に変換 |
| `env_free(env)` | メモリ解放 |

**なぜ `char **envp` をそのまま使わないのか:** `envp` は起動時の固定長スナップショット。`export` で追加、`unset` で削除するたびにサイズが変わるため、連結リストの方が扱いやすい。ただし `execve` は `char **` を要求するので、実行のたびに `env_to_array()` で変換する。

### t_shell — 全モジュールが参照する共有コンテキスト

グローバル変数を使わずに共有状態を渡すための入れ物。全関数にポインタで渡す。

```c
typedef struct s_shell
{
    t_env   *env;           // 環境変数の連結リスト
    int     last_status;    // 直前コマンドの終了ステータス（$? 用）
}   t_shell;
```

| 参照元 | アクセス |
|--------|----------|
| Expander | `env` と `last_status` を読み取り（`$VAR`、`$?` の展開） |
| Executor | `last_status` を書き込み |
| Builtins 全般 | `env` を参照 |
| `cd`, `export`, `unset` | `env` を直接変更 |

**唯一のグローバル変数:**

```c
volatile sig_atomic_t g_signal;
```

シグナルハンドラは OS が非同期に呼び出すため、関数引数で状態を渡せない。`volatile sig_atomic_t` 型でシグナル番号だけを安全に保存する。これ以外のグローバル変数は 42 Norm で禁止。

---

## 各フェーズの詳細

### Phase 0: 共通基盤（二人で実施）

プロジェクト骨格、Makefile、全データ構造の定義、環境変数管理関数、`main.c` の REPL ループスケルトンを作成する。ここで合意した `t_token` と `t_cmd` が、以降の並行開発の「契約」になる。

### Phase 1-A: パーシング（Person A）

#### Lexer

入力文字列を1文字ずつ読み進め、3つの状態を追跡する:

- `STATE_NORMAL` — クォート外。メタ文字（`|`, `<`, `>`, 空白）を解釈する。
- `STATE_SQUOTE` — シングルクォート内。全てリテラル扱い。
- `STATE_DQUOTE` — ダブルクォート内。`$` だけ特殊扱い（ただし展開はここではやらない）。

重要なポイント:
- `he"ll"o` のように隣接するクォート付き/なしの部分は1つの `TK_WORD` として結合する。
- `\` と `;` は subject で「解釈しない」とあるが、これはエラーではなく通常文字として扱う意味。
- 閉じられていないクォートはエラー。

#### Parser

トークン列を `TK_PIPE` で分割し、各コマンドの `argv` と `redirs` を構築する。

文法（Mandatory 部分）:

```
pipeline     : command ('|' command)*
command      : (redirection | WORD)+
redirection  : ('<' | '>' | '<<' | '>>') WORD
```

シンタックスエラーの検出: 先頭パイプ、連続パイプ、リダイレクト後に WORD がない場合などをエラーとし、`$?` を `2` に設定する。

#### Expander

パース済みの `t_cmd` の中の文字列に対して、以下の順で処理する:

1. **変数展開** — `$VAR` を `env_get` で値に置換。`$?` を `last_status` で置換。シングルクォート内は展開しない。
2. **クォート除去** — 引用符を取り除く。

### Phase 1-B: 実行エンジン（Person B）

#### 基本実行

`fork()` で子プロセスを作り、`execve()` でコマンドを実行する。PATH の解決は、コマンド名に `/` が含まれる場合はそのまま、含まれない場合は `$PATH` を `:` で分割して各ディレクトリで `access(X_OK)` を確認する。

終了ステータスの規則:
- 正常終了: `WEXITSTATUS(status)`
- シグナルで終了: `128 + WTERMSIG(status)`
- コマンドが見つからない: `127`
- パーミッションエラー: `126`

#### Builtins

7つの組み込みコマンドは `fork` しないで親プロセスで直接実行する（パイプ内を除く）。`cd` や `export` が親プロセスの状態を変更する必要があるため。

| コマンド | 主な仕様 |
|----------|----------|
| `echo` | `-n` オプション対応。`-nnn` も有効、`-nab` は無効 |
| `cd` | 引数なしで `$HOME` に移動。成功時に `PWD`/`OLDPWD` を更新 |
| `pwd` | `getcwd()` で現在のディレクトリを出力 |
| `export` | 引数なしで `declare -x` 形式の一覧表示。引数ありで環境変数を設定 |
| `unset` | 指定された環境変数を削除。存在しないキーはエラーにしない |
| `env` | `value` がある環境変数のみ `KEY=VALUE` 形式で出力 |
| `exit` | 引数なしで `last_status` で終了。数値でなければエラー。引数2つ以上はエラーだが終了しない |

#### リダイレクション

`t_redir` リストを左から右に順番に処理し、`open()` + `dup2()` で fd をすり替える。単一 builtin の場合は `dup()` で stdin/stdout を退避し、実行後に復元する。

#### パイプ

`cmd->next` が存在する場合に `pipe()` を作成。子プロセスで `dup2(prev_fd, STDIN)` と `dup2(pipe_fd[1], STDOUT)` を行い、親プロセスでは使わない fd を即座に `close` する。fd リークを防ぐことが最も重要。

### Phase 2: 結合（二人で実施）

#### パイプライン統合

`main.c` のループで `lexer → parser → expand → execute` を接続する。

#### シグナルハンドリング

3つの場面で設定を切り替える:

| 場面 | SIGINT (ctrl-C) | SIGQUIT (ctrl-\\) |
|------|-----------------|-------------------|
| プロンプト待ち | 新しいプロンプトを表示 | 無視 |
| 子プロセス実行中 | 子プロセスに届く（親は無視） | 子プロセスに届く |
| heredoc 入力中 | 中断してプロンプトに戻る | 無視 |

シグナルハンドラ内では `g_signal` にシグナル番号を保存するだけ。`readline` 後に `g_signal == SIGINT` なら `last_status = 130` に設定する。

#### Heredoc

`pipe()` を作成し、`readline(">")` でデリミタが来るまで入力を読み取り、`pipe_fd[0]` を返す。シグナル対応のため、heredoc は子プロセスで実行し、`ctrl-C` でプロセスが終了したら親が検知して中断する方式が最もクリーン。

### Phase 3: テスト・品質（二人で実施）

- `valgrind --leak-check=full` で自前コードのメモリリークを確認（`readline` 由来は無視可）
- Norminette チェック（25行/関数、80文字/行、変数宣言は先頭など）
- エッジケーステスト（空入力、空コマンド、`argv[0] == NULL` の場合の segfault 防止）

---

## 分業とタスク管理

### 分業方針

Person A がパーシング（Lexer → Parser → Expander）、Person B が実行エンジン（fork/execve → Builtins → リダイレクション → パイプ）を担当する。Phase 0 で合意したデータ構造（`t_token`、`t_cmd`）が二人の「契約」であり、Phase 2 の結合まで完全に並行して開発できる。

### タスク分割の原則

- 1 PR = 1 タスクが意味のある粒度
- 1 レビューが 15〜30 分で完了する規模
- 依存関係を明確にし、前のタスクがマージされてから次に進む

### タスク一覧（全 33 タスク）

| # | タスク | 担当 | 見積もり | 依存 |
|---|--------|------|----------|------|
| **Phase 0: 共通基盤** | | | | |
| 0-1 | プロジェクト骨格の作成 | 共同 | ~80行 | — |
| 0-2 | 共通データ構造の定義 | 共同 | ~60行 | 0-1 |
| 0-3 | 環境変数の初期化と基本操作 | 共同 | ~120行 | 0-2 |
| 0-4 | env_to_array と環境変数ユーティリティ | 共同 | ~80行 | 0-3 |
| 0-5 | main.c のスケルトンと REPL ループ | 共同 | ~50行 | 0-3 |
| **Phase 1-A: Lexer** | | | | |
| 1A-1 | Lexer コア — 空白スキップとメインループ | A | ~80行 | 0-2 |
| 1A-2 | 演算子トークンの読み取り | A | ~60行 | 1A-1 |
| 1A-3 | ワードトークンの読み取り（クォート対応） | A | ~100行 | 1A-1 |
| 1A-4 | Lexer のユニットテストとデバッグ出力 | A | ~70行 | 1A-3 |
| **Phase 1-A: Parser** | | | | |
| 1A-5 | Parser コア — パイプラインの分割 | A | ~80行 | 1A-1 |
| 1A-6 | コマンドのパース（argv + redir 振り分け） | A | ~100行 | 1A-5 |
| 1A-7 | リダイレクトのパースとシンタックスエラー検出 | A | ~90行 | 1A-6 |
| 1A-8 | Parser のデバッグ出力とテスト | A | ~60行 | 1A-7 |
| **Phase 1-A: Expander** | | | | |
| 1A-9 | 変数展開のコア処理 | A | ~120行 | 1A-5, 0-3 |
| 1A-10 | クォート除去 | A | ~60行 | 1A-9 |
| 1A-11 | Expander の全体適用とテスト | A | ~80行 | 1A-10 |
| **Phase 1-B: 基本実行エンジン** | | | | |
| 1B-1 | PATH の解決（コマンド検索） | B | ~90行 | 0-3 |
| 1B-2 | 単純コマンドの実行（fork + execve） | B | ~100行 | 1B-1 |
| 1B-3 | ハードコード入力での Executor テスト | B | ~50行 | 1B-2 |
| **Phase 1-B: Builtins 前半** | | | | |
| 1B-4 | builtin 判定と echo の実装 | B | ~80行 | 0-2 |
| 1B-5 | pwd と env の実装 | B | ~50行 | 0-3 |
| 1B-6 | cd の実装 | B | ~80行 | 0-3 |
| **Phase 1-B: Builtins 後半** | | | | |
| 1B-7 | export の実装 | B | ~100行 | 0-4 |
| 1B-8 | unset の実装 | B | ~30行 | 0-3 |
| 1B-9 | exit の実装 | B | ~70行 | 0-2 |
| **Phase 1-B: リダイレクションとパイプ** | | | | |
| 1B-10 | リダイレクションの適用 | B | ~80行 | 0-2 |
| 1B-11 | 単一 builtin のリダイレクション対応 | B | ~60行 | 1B-10, 1B-4 |
| 1B-12 | パイプラインの実行 | B | ~120行 | 1B-2, 1B-10 |
| 1B-13 | 実行エントリポイントと子プロセス待機 | B | ~70行 | 1B-12, 1B-11 |
| **Phase 2: 結合** | | | | |
| 2-1 | main.c のパイプライン統合 | 共同 | ~40行 | 1A-11, 1B-13 |
| 2-2 | 結合テストと修正 | 共同 | ~50行 | 2-1 |
| 2-3 | インタラクティブモードのシグナル設定 | 共同 | ~70行 | 0-5 |
| 2-4 | 子プロセス実行中のシグナル設定 | 共同 | ~50行 | 2-3 |
| 2-5 | g_signal のメインループ統合 | 共同 | ~30行 | 2-4 |
| 2-6 | 基本的な heredoc の実装 | 共同 | ~80行 | 1B-10 |
| 2-7 | heredoc のシグナル対応（子プロセス方式） | 共同 | ~70行 | 2-6, 2-4 |
| **Phase 3: テスト・品質** | | | | |
| 3-1 | エッジケースの修正（空入力・空コマンド） | 共同 | ~60行 | 2-2 |
| 3-2 | メモリリーク修正 | 共同 | ~80行 | 3-1 |
| 3-3 | Norminette 修正 | 共同 | ~100行 | 3-2 |
| 3-4 | README.md の作成 | 共同 | ~60行 | 3-3 |

---

## 参考資料

### 技術文献

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/) — 挙動に迷ったときの公式リファレンス
- [The Open Group Base Specifications — Shell Command Language](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html) — POSIX シェルの仕様
- [Writing Your Own Shell (Purdue CS)](https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf) — シェル実装の教科書的資料
