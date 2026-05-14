# Executor OS Guide

このガイドは、`minishell` の `executor` を実装する前に知っておくべき
OS レベルの基礎を、初学者向けに整理したものです。

対応するサンプルコードは `docs/os_samples/` にあります。

```bash
make -C docs/os_samples
./docs/os_samples/01_fork_wait
./docs/os_samples/02_execve
./docs/os_samples/03_dup2_redir
./docs/os_samples/04_pipe
./docs/os_samples/05_signal
./docs/os_samples/06_path_search ls
```

## 1. executor の仕事

`executor` の役割は、`parser` が作った `t_cmd` の連結リストを、
実際のプロセス実行へ変換することです。

あなたが最終的に作ることになる処理は、だいたい次の順です。

1. 実行するコマンドのパスを解決する
2. 必要なら `fork()` で子プロセスを作る
3. 子プロセス側で `dup2()` を使って入出力を差し替える
4. 子プロセス側で `execve()` を呼ぶ
5. 親プロセス側で `waitpid()` して終了ステータスを回収する

ここで重要なのは、
`execve()` は「別プログラムを呼び出す関数」ではなく、
「今のプロセスの中身を別プログラムで置き換える関数」だということです。

そのため、普通はこうなります。

1. 親が `fork()`
2. 子が `execve()`
3. 親が `waitpid()`

## 2. プロセスとは何か

プロセスは、実行中のプログラムです。

例えば `ls` を起動すると、OS は `ls` というプログラムのために
1つのプロセスを作ります。

各プロセスには PID があります。

- 親プロセス: 子を作る側
- 子プロセス: `fork()` によって作られる側

`minishell` では、シェル本体が親プロセスで、
外部コマンドは子プロセスとして動くのが基本です。

## 3. fork()

`fork()` は、現在のプロセスを複製します。

戻り値の意味:

- `-1`: 失敗
- `0`: 子プロセス側
- `> 0`: 親プロセス側で、値は子の PID

`fork()` の直後は、親も子も同じコード位置から実行を続けます。
違いは戻り値だけです。

最小イメージ:

```c
pid = fork();
if (pid < 0)
    error;
if (pid == 0)
{
    // 子プロセス
}
else
{
    // 親プロセス
}
```

ポイント:

- 親と子は「別プロセス」
- 変数は最初は同じ値を持つが、その後は互いに影響しない
- 子が `exit()` しても親は残る

対応サンプル:

- `docs/os_samples/01_fork_wait.c`

## 4. execve()

`execve()` は、現在のプロセスの中身を別プログラムに置き換えます。

```c
execve(path, argv, envp);
```

引数:

- `path`: 実行ファイルのフルパス
- `argv`: コマンドライン引数配列
- `envp`: 環境変数配列

例:

```c
char *argv[] = {"/bin/echo", "hello", NULL};
char *envp[] = {"DEMO=42", NULL};
execve("/bin/echo", argv, envp);
```

重要:

- 成功したら戻らない
- 戻ってきたら失敗
- 失敗時は `perror("execve")` で原因を確認する

なぜ `fork()` の後に子で呼ぶのか:

- 親が直接 `execve()` すると、`minishell` 自体が消える
- 子で `execve()` すれば、親シェルは残る

対応サンプル:

- `docs/os_samples/02_execve.c`

## 5. waitpid() と終了ステータス

親は、子が終わるのを待って結果を回収します。

```c
pid_t waitpid(pid_t pid, int *status, int options);

int status;

waitpid(pid, &status, 0);
```

### 引数の意味

#### 第1引数 `pid`

どの子プロセスを待つかを指定します。

- `pid > 0`: その PID の子だけを待つ
- `pid == -1`: どの子でもよいので、終了した子を1つ待つ

`minishell` では、
「このコマンドで作った特定の子を待ちたい」ことが多いので、
まずは `pid > 0` の形だけ理解すれば十分です。

#### 第2引数 `status`

子プロセスの終了情報を受け取るための `int` へのポインタです。

```c
int status;

waitpid(pid, &status, 0);
```

ここで渡すのは `status` 自体ではなく `&status` です。
理由は、`waitpid()` がこの変数の中身を書き換えるからです。

この値はそのままでは読みにくいので、
`WIFEXITED(status)` や `WEXITSTATUS(status)` のような
マクロを使って解釈します。

#### 第3引数 `options`

待ち方の指定です。

- `0`: 普通に待つ。子が終わるまで親はここで止まる
- `WNOHANG`: 子がまだ終わっていなければ、待たずにすぐ戻る

`minishell` の基本実装では、まず `0` だけ使えば足ります。

```c
waitpid(pid, &status, 0);
```

これは
「`pid` の子が終了するまで待って、結果を `status` に入れてほしい」
という意味です。

### 返り値

- `> 0`: 終了状態を回収した子の PID
- `0`: `WNOHANG` を付けていて、まだ終了した子がいない
- `-1`: エラー

`minishell` では、
`waitpid()` が `-1` を返したときのエラー処理も必要ですが、
最初は「成功したら `status` を読む」と理解すれば十分です。

`status` はそのままでは分かりにくいので、マクロで読む必要があります。

- `WIFEXITED(status)`: 子が `return` または `exit()` で終わったか
- `WEXITSTATUS(status)`: その終了コード
- `WIFSIGNALED(status)`: シグナルで終わったか
- `WTERMSIG(status)`: どのシグナルで終わったか

`minishell` では、これを `$?` に反映します。

よく使う規則:

- 普通に `exit(0)` したら `0`
- 普通に `exit(42)` したら `42`
- `SIGINT` で死んだら `130`
- `SIGQUIT` で死んだら `131`

これは `128 + signal number` という bash 互換のルールです。

最小形としては、こう読めば十分です。

```c
int	status;

waitpid(pid, &status, 0);
if (WIFEXITED(status))
	printf("exit code = %d\n", WEXITSTATUS(status));
else if (WIFSIGNALED(status))
	printf("signal status = %d\n", 128 + WTERMSIG(status));
```

対応サンプル:

- `docs/os_samples/01_fork_wait.c`
- `docs/os_samples/05_signal.c`

## 6. ファイルディスクリプタと dup2()

Unix では、入出力はファイルディスクリプタで扱います。

- `0`: `STDIN_FILENO`
- `1`: `STDOUT_FILENO`
- `2`: `STDERR_FILENO`

例えば `printf()` は最終的に `stdout` へ流れます。
その `stdout` の実体は fd `1` です。

### open()

ファイルを開くと、新しい fd が返ります。

```c
fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

### dup2()

`dup2(oldfd, newfd)` は、
`newfd` を `oldfd` と同じ先へ向けます。

```c
dup2(fd, STDOUT_FILENO);
```

これをやると、以後の標準出力はファイルへ流れます。

### close()

使い終わった fd は必ず閉じます。

fd を閉じ忘れると:

- fd leak が起こる
- pipe の EOF が来なくなる
- デバッグが難しくなる

### builtin のリダイレクトで必要なこと

単一 builtin を親プロセスで実行する場合、
`stdout` を一時的に差し替えて、終わったら戻す必要があります。

流れ:

1. `saved_stdout = dup(STDOUT_FILENO)`
2. `dup2(file_fd, STDOUT_FILENO)`
3. builtin 実行
4. `dup2(saved_stdout, STDOUT_FILENO)`
5. `close(saved_stdout)`

対応サンプル:

- `docs/os_samples/03_dup2_redir.c`

## 7. pipe()

pipe は、「あるプロセスの出力を、別のプロセスの入力へ渡す仕組み」です。

```c
int pipe_fd[2];
pipe(pipe_fd);
```

意味:

- `pipe_fd[0]`: 読み取り側
- `pipe_fd[1]`: 書き込み側

たとえば `echo hello | cat` は、
前のコマンドの `stdout` を pipe の書き込み側へ、
後ろのコマンドの `stdin` を pipe の読み取り側へ繋ぎます。

### 基本の流れ

前段の子:

```c
dup2(pipe_fd[1], STDOUT_FILENO);
close(pipe_fd[0]);
close(pipe_fd[1]);
```

後段の子:

```c
dup2(pipe_fd[0], STDIN_FILENO);
close(pipe_fd[0]);
close(pipe_fd[1]);
```

親:

```c
close(pipe_fd[0]);
close(pipe_fd[1]);
```

### なぜ close が重要か

書き込み側 fd がどこかに残っている限り、
読み取り側は EOF を受け取りません。

つまり、不要な fd を閉じ忘れると、
`cat | cat` のようなコマンドが止まらなくなることがあります。

対応サンプル:

- `docs/os_samples/04_pipe.c`

## 8. signal

`minishell` では `Ctrl-C` や `Ctrl-\` を正しく扱う必要があります。

ここで使うのがシグナルです。

よく出るもの:

- `SIGINT`: `Ctrl-C`
- `SIGQUIT`: `Ctrl-\`

### sigaction()

シグナルの扱いを設定します。

```c
struct sigaction sa;

sa.sa_handler = handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGINT, &sa, NULL);
```

設定の考え方:

- 対話中の親: `SIGINT` で新しいプロンプトへ戻る
- 子プロセス: デフォルト動作に戻す
- 親の待機中: 自分は無視して、子に任せる

これを分けないと、`Ctrl-C` で親と子が同時に変な状態になります。

対応サンプル:

- `docs/os_samples/05_signal.c`

## 9. PATH と access()

コマンド実行では、`ls` のような名前を
`/bin/ls` のような実際のパスへ変換する必要があります。

ルール:

- `cmd` に `/` があるなら、そのまま相対/絶対パス扱い
- `/` がないなら `PATH` を探す

`PATH` はこういう文字列です。

```text
/usr/local/bin:/usr/bin:/bin
```

これを `:` で分割して、
各ディレクトリに `cmd` を繋げて `access(..., X_OK)` で確認します。

```c
if (access(full_path, X_OK) == 0)
    return (full_path);
```

注意:

- `PATH` が unset なら探索しない
- `command not found` と `permission denied` は同じではない

対応サンプル:

- `docs/os_samples/06_path_search.c`

## 10. minishell への対応づけ

`executor` 実装では、だいたい次の関数に分かれます。

### 1B-1: PATH 解決

- `resolve_path(cmd, shell)`
- `env_get(shell->env, "PATH")`
- `ft_split(path_env, ':')`
- `access(path, X_OK)`

### 1B-2: 単純コマンド実行

1. builtin か判定
2. 外部コマンドなら `fork()`
3. 子で `resolve_path()`
4. 子で `env_to_array()`
5. 子で `execve()`
6. 親で `waitpid()`

### 1B-10: リダイレクト

- `open()`
- `dup2()`
- `close()`

### 1B-12: パイプライン

- `pipe()`
- `fork()`
- `dup2(prev_fd, STDIN_FILENO)`
- `dup2(pipe_fd[1], STDOUT_FILENO)`
- 不要 fd を全部 `close()`

### 2-4: 子実行中の signal

- 親待機中は `SIGINT` と `SIGQUIT` を調整
- 子は `SIG_DFL`
- `waitpid()` 後に親の対話モードへ戻す

## 11. 学ぶ順番

初学者なら次の順で触るのが最も理解しやすいです。

1. `01_fork_wait.c`
2. `02_execve.c`
3. `03_dup2_redir.c`
4. `04_pipe.c`
5. `05_signal.c`
6. `06_path_search.c`

この順で理解すると、
`executor` の設計が「難しい関数の集合」ではなく、
「小さな OS 操作を組み合わせたもの」に見えるようになります。

## 12. 次にやるべきこと

このガイドを読んだ直後に着手すべき実装は次です。

1. `resolve_path()` を実装する
2. `exec_simple_cmd()` を作る
3. builtin 判定関数を作る
4. `apply_redirections()` を作る
5. `exec_pipeline()` を作る

もし `fork()` や `dup2()` の感覚がまだ曖昧なら、
先にサンプルを動かして挙動を目で確認した方が速いです。
