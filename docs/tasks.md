# Minishell タスク一覧

## Phase 0: 共通基盤（共同）

- [ ] **0-1: プロジェクト骨格の作成** `共同` `~80行`
  - ファイル: `Makefile`, `includes/minishell.h`
  - ディレクトリ構成、Makefile（NAME/all/clean/fclean/re）、メインヘッダを作成。libft のコピーと統合。

- [ ] **0-2: 共通データ構造の定義** `共同` `~60行`
  - ファイル: `includes/minishell.h`
  - 依存: 0-1
  - t_token, t_token_type(enum), t_cmd, t_redir, t_shell の構造体定義。全メンバーと型を確定。

- [ ] **0-3: 環境変数の初期化と基本操作** `共同` `~120行`
  - ファイル: `src/env/env_init.c`, `src/env/env_ops.c`, `includes/env.h`
  - 依存: 0-2
  - t_env 構造体定義。env_init(&env, envp→linked list変換)、env_get、env_set、env_unset、env_free を実装。

- [ ] **0-4: env_to_array と環境変数ユーティリティ** `共同` `~80行`
  - ファイル: `src/env/env_utils.c`
  - 依存: 0-3
  - env_to_array（execve用のchar\*\*変換）、env_print_export（declare -x形式の出力）。split_key_value ヘルパー。

- [ ] **0-5: main.c のスケルトンとREPLループ** `共同` `~50行`
  - ファイル: `src/main.c`
  - 依存: 0-3
  - g_signal宣言、readline+add_historyのループ、ctrl-D(EOF)でのexit処理。各フェーズの呼び出しはコメントアウト。

---

## Phase 1-A: Lexer（Person A）

- [ ] **1A-1: Lexerコア — 空白スキップとメインループ** `Person A` `~80行`
  - ファイル: `src/lexer/lexer.c`, `includes/lexer.h`
  - 依存: 0-2
  - lexer()関数の骨格。skip_whitespace、トークンリストの構築ループ、new_token、add_token、free_tokens。

- [ ] **1A-2: 演算子トークンの読み取り** `Person A` `~60行`
  - ファイル: `src/lexer/lexer_operator.c`
  - 依存: 1A-1
  - read_operator(): |, <, >, <<, >> の判定と TK_PIPE/TK_REDIR_IN/TK_REDIR_OUT/TK_HEREDOC/TK_APPEND トークン生成。

- [ ] **1A-3: ワードトークンの読み取り（クォート対応）** `Person A` `~100行`
  - ファイル: `src/lexer/lexer_word.c`
  - 依存: 1A-1
  - read_word(): STATE_NORMAL/STATE_SQUOTE/STATE_DQUOTE の状態遷移。隣接クォート（he"ll"o）の結合。閉じられていないクォートのエラー検出。

- [ ] **1A-4: Lexerのユニットテストとデバッグ出力** `Person A` `~70行`
  - ファイル: `src/lexer/lexer_debug.c`
  - 依存: 1A-3
  - debug_print_tokens()。基本ケース（echo hello）、クォート、パイプ、リダイレクト、エラーケースのテスト。

## Phase 1-A: Parser（Person A）

- [ ] **1A-5: Parserコア — パイプラインの分割** `Person A` `~80行`
  - ファイル: `src/parser/parser.c`, `includes/parser.h`
  - 依存: 1A-1
  - parser()関数: トークン列をTK_PIPEで分割し、t_cmd の連結リストを構築するメインループ。free_cmds。

- [ ] **1A-6: コマンドのパース（argv + redir振り分け）** `Person A` `~100行`
  - ファイル: `src/parser/parse_command.c`
  - 依存: 1A-5
  - parse_command(): TK_WORDをargvに、リダイレクトトークンをt_redirリストに振り分ける。argv配列のNULL終端。

- [ ] **1A-7: リダイレクトのパースとシンタックスエラー検出** `Person A` `~90行`
  - ファイル: `src/parser/parse_redir.c`, `src/parser/parse_error.c`
  - 依存: 1A-6
  - parse_redir(): リダイレクトの次がTK_WORDか検証。syntax_error(): 連続パイプ、先頭パイプ、リダイレクト後の非WORD等のエラー出力。$?=2。

- [ ] **1A-8: Parserのデバッグ出力とテスト** `Person A` `~60行`
  - ファイル: `src/parser/parser_debug.c`
  - 依存: 1A-7
  - debug_print_cmds()。パイプ、複合リダイレクト、エラーケース（| |, >, <<）のテスト確認。

## Phase 1-A: Expander（Person A）

- [ ] **1A-9: 変数展開のコア処理** `Person A` `~120行`
  - ファイル: `src/expander/expander.c`, `includes/expander.h`
  - 依存: 1A-5, 0-3
  - expand_str(): クォート状態を追跡しつつ $VAR を env_get で展開。$? を last_status で展開。変数名の切れ目判定（英数字+_）。

- [ ] **1A-10: クォート除去** `Person A` `~60行`
  - ファイル: `src/expander/quote_removal.c`
  - 依存: 1A-9
  - remove_quotes(): 展開後の文字列からシングル/ダブルクォートを除去。クォート状態の追跡。

- [ ] **1A-11: Expanderの全体適用とテスト** `Person A` `~80行`
  - ファイル: `src/expander/expand_cmd.c`
  - 依存: 1A-10
  - expand(): t_cmd リスト全体の argv と redir->file に expand_str + remove_quotes を適用。$VAR, $?, クォート内外のテスト。

---

## Phase 1-B: 基本実行エンジン（Person B）

- [ ] **1B-1: PATHの解決（コマンド検索）** `Person B` `~90行`
  - ファイル: `src/executor/path_resolve.c`, `includes/executor.h`
  - 依存: 0-3
  - resolve_path(): / を含む場合はそのまま、含まない場合はPATHをsplitして各ディレクトリでaccess(X_OK)。PATH未設定時の処理。

- [ ] **1B-2: 単純コマンドの実行（fork+execve）** `Person B` `~100行`
  - ファイル: `src/executor/exec_simple.c`
  - 依存: 1B-1
  - exec_simple_cmd(): fork→子でexecve、親でwaitpid。終了ステータスの取得（WIFEXITED/WIFSIGNALED）。command not found(127)、permission denied(126)。

- [ ] **1B-3: ハードコード入力でのExecutorテスト** `Person B` `~50行`
  - ファイル: `src/executor/exec_test.c`
  - 依存: 1B-2
  - テスト用ハーネス: 手動で t_cmd を構築して exec_simple_cmd を呼び出す。ls -la, /bin/echo hello, 存在しないコマンドのテスト。

## Phase 1-B: Builtins 前半（Person B）

- [ ] **1B-4: builtin判定と echo の実装** `Person B` `~80行`
  - ファイル: `src/builtins/builtins.c`, `src/builtins/builtin_echo.c`, `includes/builtins.h`
  - 依存: 0-2
  - is_builtin()、exec_builtin()のディスパッチ。builtin_echo(): -n フラグの判定（-nnn も有効、-nab は無効）。

- [ ] **1B-5: pwd と env の実装** `Person B` `~50行`
  - ファイル: `src/builtins/builtin_pwd.c`, `src/builtins/builtin_env.c`
  - 依存: 0-3
  - builtin_pwd(): getcwd+出力。builtin_env(): t_env を走査し value があるもののみ KEY=VALUE 形式で出力。

- [ ] **1B-6: cd の実装** `Person B` `~80行`
  - ファイル: `src/builtins/builtin_cd.c`
  - 依存: 0-3
  - builtin_cd(): 引数なし→$HOME、chdir()、成功時にPWDとOLDPWDを更新。引数過多エラー。HOME未設定エラー。

## Phase 1-B: Builtins 後半（Person B）

- [ ] **1B-7: export の実装** `Person B` `~100行`
  - ファイル: `src/builtins/builtin_export.c`
  - 依存: 0-4
  - 引数なし→ソート済みdeclare -x出力。引数あり→is_valid_identifier検証、split_key_value、env_set。無効な識別子のエラー出力。

- [ ] **1B-8: unset の実装** `Person B` `~30行`
  - ファイル: `src/builtins/builtin_unset.c`
  - 依存: 0-3
  - builtin_unset(): 引数を順に env_unset。存在しないキーはエラーにしない。

- [ ] **1B-9: exit の実装** `Person B` `~70行`
  - ファイル: `src/builtins/builtin_exit.c`
  - 依存: 0-2
  - 引数なし→last_statusで終了。数値判定（is_numeric）、オーバーフロー対応（ft_atoll）。引数2つ以上→エラーだが終了しない。unsigned char でmod 256。

## Phase 1-B: リダイレクションとパイプ（Person B）

- [ ] **1B-10: リダイレクションの適用** `Person B` `~80行`
  - ファイル: `src/executor/redir.c`
  - 依存: 0-2
  - apply_redirections(): t_redir リストを左から右に処理。open + dup2 + close。O_RDONLY / O_WRONLY|O_CREAT|O_TRUNC / O_APPEND。エラー時のperror。

- [ ] **1B-11: 単一builtinのリダイレクション対応** `Person B` `~60行`
  - ファイル: `src/executor/exec_builtin_redir.c`
  - 依存: 1B-10, 1B-4
  - exec_builtin_with_redir(): dup で stdin/stdout を退避、apply_redirections、builtin実行、dup2 で復元。パイプ外のbuiltin用。

- [ ] **1B-12: パイプラインの実行** `Person B` `~120行`
  - ファイル: `src/executor/exec_pipeline.c`
  - 依存: 1B-2, 1B-10
  - exec_pipeline(): cmd->nextがある場合にpipe()作成。子プロセスでdup2(prev_fd, STDIN) / dup2(pipe_fd[1], STDOUT)。親でのfd close管理。

- [ ] **1B-13: 実行エントリポイントと子プロセス待機** `Person B` `~70行`
  - ファイル: `src/executor/executor.c`, `src/executor/wait.c`
  - 依存: 1B-12, 1B-11
  - execute(): 単一builtin判定→fork不要パス、それ以外→exec_pipeline。wait_all(): 全子プロセスのwaitpid、最後のコマンドのステータスを返す。

---

## Phase 2: 結合（共同）

- [ ] **2-1: main.c のパイプライン統合** `共同` `~40行`
  - ファイル: `src/main.c`
  - 依存: 1A-11, 1B-13
  - mainループで lexer → parser → expand → execute を接続。各フェーズの戻り値チェックとエラー時のcontinue。メモリ解放の確認。

- [ ] **2-2: 結合テストと修正（基本コマンド）** `共同` `~50行`
  - ファイル: `src/main.c`
  - 依存: 2-1
  - echo hello, ls | grep, < infile cat > outfile, export/unset/env の一連のテスト。不整合の修正。

## Phase 2: シグナルハンドリング（共同）

- [ ] **2-3: インタラクティブモードのシグナル設定** `共同` `~70行`
  - ファイル: `src/signals/signals.c`, `includes/signals.h`
  - 依存: 0-5
  - setup_signals_interactive(): SIGINT→新プロンプト（rl_on_new_line, rl_replace_line, rl_redisplay）、SIGQUIT→SIG_IGN。sigaction使用。

- [ ] **2-4: 子プロセス実行中のシグナル設定** `共同` `~50行`
  - ファイル: `src/signals/signals.c`
  - 依存: 2-3
  - setup_signals_child(): SIG_DFL復元。setup_signals_parent_wait(): 親はSIGINT/SIGQUIT無視。fork前後での切り替え。SIGQUIT時の"Quit (core dumped)"出力。

- [ ] **2-5: g_signal のメインループ統合** `共同` `~30行`
  - ファイル: `src/main.c`
  - 依存: 2-4
  - readline後に g_signal==SIGINT なら last_status=130, g_signal=0。シグナル状態のリセット。

## Phase 2: Heredoc（共同）

- [ ] **2-6: 基本的なheredocの実装** `共同` `~80行`
  - ファイル: `src/executor/heredoc.c`
  - 依存: 1B-10
  - handle_heredoc(): pipe()作成、readline(">")のループ、デリミタ一致で終了、pipe_fd[0]を返す。apply_redirectionsからの呼び出し。

- [ ] **2-7: heredocのシグナル対応（子プロセス方式）** `共同` `~70行`
  - ファイル: `src/executor/heredoc.c`
  - 依存: 2-6, 2-4
  - handle_heredoc_safe(): heredocを子プロセスで実行。ctrl-C でプロセス終了→親が検知して中断。デリミタにクォートがある場合は変数展開を無効化。

---

## Phase 3: テスト・品質（共同）

- [ ] **3-1: エッジケースの修正（空入力・空コマンド）** `共同` `~60行`
  - ファイル: `src/lexer/lexer.c`, `src/parser/parser.c`, `src/executor/executor.c`
  - 依存: 2-2
  - 空行、空白のみ、""|""、argv[0]==NULLの場合のNULLチェック追加。segfault防止。

- [ ] **3-2: メモリリーク修正** `共同` `~80行`
  - ファイル: 各ファイル
  - 依存: 3-1
  - valgrind で検出されたリークの修正。ft_strjoin中間文字列、エラーパスのearly return、env_to_array、getcwd の free漏れ。

- [ ] **3-3: Norminette修正** `共同` `~100行`
  - ファイル: 各ファイル
  - 依存: 3-2
  - 25行超の関数の分割、80文字超の行の整形、変数宣言の先頭移動、for→while変換。

- [ ] **3-4: README.md の作成** `共同` `~60行`
  - ファイル: `README.md`
  - 依存: 3-3
  - 42フォーマット: イタリック行、Description、Instructions(make/実行)、Resources(参考文献+AI使用記述)。英語。
