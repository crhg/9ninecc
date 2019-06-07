# 開発記録

## 2019/05/29

### [整数1つをコンパイルするコンパイラを作成](https://github.com/crhg/9ninecc/commit/15842e37b59a5b7ab8379e7667e86238b84b8924)

基本そのまま写経レベル。

### [足し算と引き算を追加](https://github.com/crhg/9ninecc/commit/f60357faa03e3e59b8656efed8fc5e4157b24796)

atoiからstrtolになってなるほどという感じ

### [Create README.md](https://github.com/crhg/9ninecc/commit/85a52aeac6f21e766911afbad00b2cad75a53a1)

そういえばREADMEなかったなと思って。名前の由来を書いた。

### [トークナイズするようにして空白をスキップ可能にした](https://github.com/crhg/9ninecc/commit/d702ce364ae1c9d475890e83f790ae22781f1812)

トークナイザがあるとコンパイラっぽい。

`error_at`で場所が出るのはいいね。

### [Merge branch 'master' of github.com:crhg/9ninecc](https://github.com/crhg/9ninecc/commit/62473761df76d5c9632d1edca58434095f4569c2)

前と同じような差分がまた出たのはなんでだかわからん。git難しい。

### [構文木を使ったコードジェネレータを導入(加減算のみ)](https://github.com/crhg/9ninecc/commit/96076c6f288e8d5fb66a64704be4e351fb97b848)

構文木がでてますますコンパイラらしくなった。

100個以上のトークンは来ないものとするとか割り切りが素敵。

`ND_NUM = 256`の256は何故だろうと思ったが、1文字の演算子のノードの`ty`をその文字にするためか。

`consume`の戻り値は真偽値でなく`Token *`にしてみた。

### [かけ算を追加](https://github.com/crhg/9ninecc/commit/476a9cd474b033bc8924c399fe2260463a0893cd)

演算子の優先度ごとに構文要素わけるやつ。

### [割り算を追加](https://github.com/crhg/9ninecc/commit/cd43d7abbef72ae83065edf5ce612b2dfc0703b4)

`cqo`命令ってなんだろうと思って調べたがよくみたら付録にチートシートが。

### [カッコを追加](https://github.com/crhg/9ninecc/commit/4109ccfbbccdfea512a68d42ce455e4fbeed05e1)

termって何だろう。

### [単項の'+'と'-'](https://github.com/crhg/9ninecc/commit/cd10c9c026b386c16cb96c7c6a412416aca86349)

同じ記号の演算子だと困るなーと思ったら、-xは0-xとして扱うのでノードの種類は増えないのだった。
たしかにコード生成も追加しなくていいしこれは楽。

### [比較演算子](https://github.com/crhg/9ninecc/commit/16bd1f0a33a0cfbd085e7c8ea7af2b95276e2fbb)

x86-64にはフラグからレジスタに値を設定する命令があるのか。

8bit時代はフラグで分岐するしかなかったけどパイプラインとかあると分岐したくないもんねー。

一気に演算子が増えたのでテスト書くの疲れた。

### [ファイル分割など](https://github.com/crhg/9ninecc/commit/5cd91efe6c478822494903ac453f67ba8d354a7a)

`error`や`error_at`で終わるとときどきコンパイラに怒られるので`_Noreturn`をつけた。

### [トークンの保存にベクターを使う](https://github.com/crhg/9ninecc/commit/d8bf24b6f2d8c7ed223f2e6b267304d07345a0f4)

ベクター単純なわりにつかいでがありそう。

### [1文字のローカル変数](https://github.com/crhg/9ninecc/commit/038e4cda4a87a2ec46c5b4fa7ad4d624a5d4d29b)

表題のわりにやることが盛りだくさんなステップ。

さっきベクター作ったのに`code`はまた固定配列?と思ったけど、これそのうち使わなくなりそうだしなという感じ。

コードジェネレータでエラーになるとソースのどこに対応するかわからないのはそのうち直したいと思った。

### [return文](https://github.com/crhg/9ninecc/commit/75bb0848d71b8feea1ee54c5c7ed7ede98d45869)

`is_alnum`みたいな奴標準でなかったっけと思って`isalnum`とか見つかったけど、動作がロケール依存と書いてあったのでそっ閉じ。

### [マップ](https://github.com/crhg/9ninecc/commit/88d4bfee0ffb60bdd80c9efec4bcb00bec6ad95c)

ベクターを活用して`keys`と`vals`の2本のベクター、lookupはリニアサーチという省力実装が素敵。

### [整数リテラルは整数の最小値に対応しない(いったん正の整数として解釈しているから)](https://github.com/crhg/9ninecc/commit/09d373d1b0ce3ef3d3a8728f90da6a3966402711)

どっかのslackかなにかで話題になっていたので。

### [複数文字のローカル変数](https://github.com/crhg/9ninecc/commit/6dfab008c545d6686541f4d06f2e62dc00375d41)

マップを使えば簡単。`Node`がどんどん太りそうな予感がする。

### [if文](https://github.com/crhg/9ninecc/commit/3a02af3bba1fd971d8b61c42608a2059a6af694b)

if文といいつつforやwhileのパースも書いてしまった。コード生成はifだけ。`.L`って何だろう(後でわかった)

### [while文](https://github.com/crhg/9ninecc/commit/b145d49ccc83a708ff91ab6a703a9a19687e00f9)

whileのコード生成のみ。なんかスタックが気になる。

### [while文のコード生成のバグを再現するテスト](https://github.com/crhg/9ninecc/commit/a4ce28e04a1bb866babd26c280b8bacbd0ce343d)

やはりループ毎にスタックが伸びているっぽいがあとで考えることにしてとりあえず先へ。

### [for文](https://github.com/crhg/9ninecc/commit/a2f5e64c038f8a2b77f800cfbf5b7d1fc6b85952)

for文も同じ問題がある(対処してないからねー)

文が終わったときにひとつスタックに値がある規則だとループから抜けるときと抜けないときでスタック消費をわける必要があってなんかめんどうそう。

### [式文を導入してスタック伸びる問題に対処](https://github.com/crhg/9ninecc/commit/c2014f5cd5e41c2adca8d2fc8814fafe55d912e2)

文が終わったときはスタックに値を残さない方針に変更。これで伸びなくなった。

### [ブロック](https://github.com/crhg/9ninecc/commit/611eea7faf4faee9647f44ec0089a6d1310a082a)

ベクターは便利だな。

## 2019/05/30

### [関数呼び出し(無引数)](https://github.com/crhg/9ninecc/commit/dd5ec568000ec222709814968b3575daf726bef3)

いつの時点でスタックが16バイト境界を指しているべきなのかよくわからなくていろいろ調べる。関数が呼ばれたときに16バイト境界にあればいいらしい。

呼び出し時店でのスタック位置を知るやり方は関数の先頭で16バイト境界にあったとして文の区切りではスタックが戻っているはずだから0クリアしてその後の増減を追いかける方法しか思いつかなくて愚直に実装。

自分が作った`.s`と普通の`.c`を混ぜて`gcc`に食わせればちゃんとリンクされて関数呼べるのに感動。

### [スタックの深さを管理](https://github.com/crhg/9ninecc/commit/ae74103ca88b813950ef8ad73ad57d1fc561477a)

push, popしてるところにスタック追跡をばらまく。

### [関数コール(引数あり)](https://github.com/crhg/9ninecc/commit/e43f06aa101269873f8a564a99fe18e1c62d203e)

引数ありの関数コールができるといろいろ呼べて楽しい。

引数を渡すとき最初の6つはレジスタを使うけどレジスタ名に規則性がなさすぎるので力業に。

### [関数定義(引数無し)](https://github.com/crhg/9ninecc/commit/4703e0cefa359f298f61eb097c463273eeea366d)

トップレベルが関数定義だとちゃんとしたC言語らしい。

固定長の`code`配列さようなら。

仮引数のループが微妙になってしまった。

いままでなんとなく`consume`だけで済んでいたけど、ついに次のトークンを見るだけの関数が欲しくなった。

### [関数定義(引数あり)](https://github.com/crhg/9ninecc/commit/62b35586b78c062222ccf28a8fd0c66f79aacda1)

コメントとコミット内容が全く一致していない。謎。

## 2019/05/31

### [intキーワードの導入](https://github.com/crhg/9ninecc/commit/80638a55cba121da49bf4fbe24b9ab4acc5b5ad0)

前回のコミット間違いのため、引数ありの関数定義とint導入が混じってしまった。

フィボナッチ関数の再帰版が書けるようになると確かに楽しい。

変数を使うテストの書き換えがけっこうめんどくさかった。

### [パーサのコメントをBNF風に](https://github.com/crhg/9ninecc/commit/28476967b1c75a128b0e35908bafde75e10649b5)

だいぶ複雑になってきたので、パーサの関数が扱う構文要素がどうなっているかBNF風に書いてコメントにした。

### [ローカル変数定義とパラメタ定義を共通化](https://github.com/crhg/9ninecc/commit/79881db590a690dd8f2ac311159c8ded78f3209b)

ローカル変数定義とパラメタ定義が同じ事をやっているので共通関数をくくりだしてリファクタリング。

## 2019/06/01

### [ポインタ](https://github.com/crhg/9ninecc/commit/5ff242b5b0849abc2caa5feb94ce413b27e12c94)

サイズを意識したコード生成。レジスタ名に規則性がなくてあいかわらずつらい。

けっこう込み入ってきたのでバグを仕込んで苦戦した。コード生成時でもエラー表示できるようにノードにトークンを持たせてみた。

エラー表示関数もたくさん作成。コミット分けた方がよかったような気もするけどエラー出る状態になってから作ってるのでエラー出てる間はコミットしたくないしこういうとき困る。

ローカル変数の領域割り当てや式の型付けをパースしながらやってたら混乱したので、以下のようにしてみた。

* ローカル変数の領域割り当ては関数定義の構文木ができてからやる。(パース中にはオフセットはいらなくて、コード生成までにできてればいい)

* 式の型付けは構文木ができてからやる。

### [ポインタの加算](https://github.com/crhg/9ninecc/commit/0485ab94cb70234c4f6cea1de5d30e2ce39f134c)

またもやバグを仕込んで苦戦。生成するアセンブラソースにどのノードのコード生成かをコメントとして埋め込んで追跡しやすくする試み。

ポインタと整数の加算は整数にサイズを掛け算するのだけどそのときraxを使うので型付け時に整数を左辺に入れ替えるというハック。

### [ポイント型へのポインタの加算のテストを追加](https://github.com/crhg/9ninecc/commit/fefb8eadbe05e6d6fdb1721ad6833f804861f72f)

整数型へのポインタについてしかテストしてなかったのでテスト追加。テスト用のデータを用意するプログラムの方にうっかりバグを仕込んで気付かず苦戦。本体は正しかった...

### [ポインタの減算](https://github.com/crhg/9ninecc/commit/5140dec0357e1e4f06fa3e2a83f2ef87c58d5290)

加算のときの右辺と左辺を入れ替えるハックは減算のときにはあえなく破綻するので、2項演算子共通処理から加算と減算を分離して整理。

ポインタ同士の減算もよく調べたらできるのでちゃんと書いた。

## 2019-06-02

### [new_nodeを使うようにリファクタ](https://github.com/crhg/9ninecc/commit/778be018243e543b19d4fb6203b56426d042642d)

Node構造体を直にmallocしているところが残っていたので`new_node()`を使うようにリファクタ。

### [変数定義と式の型付けはパースしながら行うようにリファクタ](https://github.com/crhg/9ninecc/commit/b1b886f5d39dae53b3aa912bc7278d613e62c64a)

型付けを後で行うようにしたのだが`sizeof`の実装で早くも不便になったので、やっぱりパースしながらやるように修正。今度は頭の中が整理されたのでなんとかなった。

### [sizeof](https://github.com/crhg/9ninecc/commit/5d20f4f0be359d128ffbffbfe118011dae37ad1e)

リファクタしたのでわりとすんなり実装。

### [配列型の定義](https://github.com/crhg/9ninecc/commit/31abf1d61ebffb16f872cee556814041d7dcca9c)

ローカル変数領域の割り当てを雑に変数の数*×8にしていたが真面目に計算するようにした。

配列型ができたのでアラインメントも計算する関数を作成。

### [配列アクセス構文](https://github.com/crhg/9ninecc/commit/d7906bd94912e380987d1e4d69c1fee61f4aef50)

`*`と`[]`の優先順位のため構文をいじった

うっかり1ステップ飛ばしたけどこれは独立しているからまあいいか。

## 2019-06-03

### [配列からポインタへの暗黙の変換](https://github.com/crhg/9ninecc/commit/79bf000a07c174081d46a8f34ddffd14a0ad35b9)

式の型を暗黙変換で書き換えるときに、うっかりTypeのtyだけ書き換えてはまる。全部のノードで共有されているので書き換えてはいけないものまで書き換わるのだった。毎回新たに生成するように修正。やや富豪的。

### [グローバル変数](https://github.com/crhg/9ninecc/commits/master)

構文解析までは難しくなかったが、どういうコードにすればいいか調べるのに手間取る。gccを参考にと書いてあるがgccの出力するアセンブラはデフォルトではインテル形式ではなかった。

以下のどちらかで。

```
gcc -S -masm=intel hoge.c
```
```
clang -S -mllvm --x86-asm-syntax=intel hoge.c
```

### [char型](https://github.com/crhg/9ninecc/commit/bcf4a91de156f3e0c89456b8053290eafd38d0aa)

パーサまでは素直にいけたけど、あいかわらずコード生成は直交性がなくてつらい。

### [文字列リテラル](https://github.com/crhg/9ninecc/commit/caa5ec6ea6855f3f0d50da7b9fd0160469b3bbe0)

調子に乗って`printf`の呼び出しに挑戦したら

```
./tmp: Symbol `printf' causes overflow in R_X86_64_PC32 relocation
```

なるエラーに悩まされる。gccにコード吐かせて見比べると、`@PLT`なる謎の記述があり真似したらOKだった。

```
   call printf@PLT
```

調べたところPLTはProcedure Linkage Tableの略で、たぶん直接飛ぶようにリンクすると遠すぎてだめだったのがテーブル経由だと大丈夫とかそんな感じじゃないかと。
テーブル経由するので近いときは直接飛ぶコードに比べて若干遅くなるだろうけどとりあえず気にしないで全部`@PLT`つきでコード生成することに。

### [入力をファイルから読む](https://github.com/crhg/9ninecc/commit/f52ae44d7d18cea861713381821f77a94bd9e77e)

ほぼコピペ。テストスクリプトで引数で直接渡していたコードを`echo`で`tmp.c`に書き込むようにしたぐらい。

### [コメント](https://github.com/crhg/9ninecc/commit/fc9e186feea9dcd8f8d12efbfcb0b39d32377c83)

ほぼコピペ。行コメントのテストがいまの`test.sh`だと面倒なのでさぼった。すぐにCで書き換えがあるのでそのときに。

## 2019-06-04

### テストをCで書き直す

けっこう大がかりになったので分割。

#### [テスト用補助ファイルの関数名衝突を解消](https://github.com/crhg/9ninecc/commit/5aea7f9206d8f9452c6ef7cdb6af57a355a8b34d)

テストから呼ぶちょっとした補助用Cコードの関数名がぶつからないように修正

#### [test.shを対応するCコードに変換する使い捨てスクリプト](https://github.com/crhg/9ninecc/commit/97faa0aeeccecb554bf6cfd322373295741b3b88)

手で書き直すのは面倒なのでperlスクリプトで変換する。ただし完璧なのを作るのは大変なのである程度は手作業前提。
インデントは`clang-format`通せばいいので雑にやる。

だいたいこんな感じに変換する。

`@try_ret`や`@try_out`はあとでこれらを呼び出すmain関数を生成するスクリプトで使う予定。

```bash
ry 42 "42;"
try_output 1-2-3-4-5-6-7-8 "foo3(1,2,3,4,5,6,7,8);" test_source/test3.c
try_output_raw "5" "int main(){pr_int(add(2,3));} int add(int a,int b){return a+b;}" test_source/print.c
```

```C
// @try_ret test1 42
int test1() { 42; }
// @end

// @try_out test41 1-2-3-4-5-6-7-8
int test41() { foo3(1, 2, 3, 4, 5, 6, 7, 8); }
// @end

// @try_out test42 "5"
int test42() { pr_int(add(2, 3)); }
int add(int a, int b) { return a + b; }
// @end
```

#### [変換したテストソースを関数名の衝突などを手当てしたもの](https://github.com/crhg/9ninecc/commit/34054879f645852681119ddb8070d10f2831de53)

変換スクリプトは`main()`だったのを`test<通番>()`に置き換えるまでは面倒見るけどそれ以上はしないので、他の関数名やグローバル変数名が衝突するときは結果を見て手で直す。

#### [既にあった補助ファイルを手直し](https://github.com/crhg/9ninecc/commit/f218b6bfa409fd4f8e5e856e8fc0359528d1c709)

`printf`を作成予定の`try_printf`に置き換える。結果をメモリに保存して比較するのに使う予定。

#### [Cで書いたテストのサポートツール](https://github.com/crhg/9ninecc/commit/2a297d5f7ce78c9b20e99205501c63504268c4da)

`test.c`から以下のように`main()`を生成する。

```C
#include "try.h"

void main(int argc, char **argv) {
  extern int test1();
  try_ret(42, test1(), "test1");

  extern int test41();
  try_out_start();
  test41();
  try_out_check("1-2-3-4-5-6-7-8", "test41");

  extern int test42();
  try_out_start();
  test42();
  try_out_check("5", "test42");

  printf("OK\n");
  exit(0);
}

```

文字列出力の場合は最初は固定バッファで行こうかと思ったけどマニュアルをみたら最近は`vasprintf`なる便利なものがあるらしいので使おうとしたらGNU拡張がなんとかいわれてうまくいかず断念。
固定バッファに書き換えるのも何なので`snprintf`+`realloc`で実装。`snprintf`は切り詰めたかどうか返してくれないのが微妙に不便。

### [C版テストで発覚した型を間違えて書き込む問題の修正](https://github.com/crhg/9ninecc/commit/f7833619adc6ab5e241da9464e7bcbf80dca894d)

C版のテストプログラムを実行させるとSEGVする。調べると最後のテスト関数までは実行されていて戻るときにおかしいらしい。
なんとなくmainの戻り番地あたりのスタックを壊している予感だがどこで壊しているのかわからないのでコメントアウト2分法で問題のあるテストを特定する。
これだと判明。

```C
// @try_ret test79 3
int test79() {
  char x[3];
  x[0] = -1;
  x[1] = 2;
  int y;
  y = 4;
  return x[0] + y;
}
```

9nineccの生成したアセンブラソースとにらめっこして、charだから

```
  mov [rax], dil
```

であるべきところが、ポインタ用の

```
  mov [rax], rdi
```

になっていた。`x`はスタックの戻り番地のすぐ上に3バイトで割り付けられているので、8バイト書いたら書きつぶしますわな。

型の受け渡しがおかしいっぽいのでどこでおかしくなるのか特定するのにデバッグ出力をばらまくなどしてかなり苦戦した。


## 2019-06-05

### 初期値の対応

いい機会なので型まわりを修正しながらやることにした。

多次元配列をはじめとした複雑な型も扱えるようになる予定。今はほぼ型を気にしていない関数もちゃんとパラメタや戻り値の型を持つようにする

#### 初期化機能ブランチ(https://github.com/crhg/9ninecc/commit/ba39fc17f5b5dd2fcab88324d1c45bf3fd86e6cf)

ちゃんと動くようになるまでだいぶかかりそうなのでブランチ分け。

[C11仕様ドラフト(N1570)](http://port70.net/~nsz/c/c11/n1570.html)を参考に主にdeclaratorのあたりを作り直す。

Type型にFUNCが入ってだいぶそれっぽくなった。

#### 初期値対応: 既存のテストはパス(https://github.com/crhg/9ninecc/commit/5123492ab5cfb9552700b9966bfdf3baa4414448)

初期化を使わない既存のテストコードが通るようになったのでいったんコミット。

ためしにCLionを使ってみることにしたのでcmakeの設定なども追加。

Mac上のCLionなので9nineccの出力するコードはそのままでは実行できないから、CLion上でテストコードをコンパイルできるようになるまではMac上で、
結果のコードの実行確認はlinuxにコピーして行うという感じの作業にした。CLionはcmakeなのでMakefileは見ないからちょうどいい。

#### インスペクターに怒られるところを修正(https://github.com/crhg/9ninecc/commit/540c91dd7a910128459320acd45ecc96bda15183)

CLionのインスペクターがあやしいところを指摘してくれるので徐々に修正。

#### 重複した宣言を削除(https://github.com/crhg/9ninecc/commit/38c2127952b90c8ae80936479ebf2321939ab06d)

CLionに宣言がだぶっていると指摘されたので削除

#### [test_sourceもcmakeに登録](https://github.com/crhg/9ninecc/commit/dc62ec3d8e518ee04fc043e0d8ea4f267ff5b8a1)

test_source下のファイルを開くとプロジェクト外のファイルだと怒られるので、こちらもcmake管理下にする。

管理下にしたらしたでビルドの時にtest_source以下もビルドされるようになってしまい、mainがないというエラー。こっちはmacでは使わないから
ビルドまではしなくていいんだけどなと思いつつ、そもそもビルド対象から外すやり方もわからないし、スクリプトで生成する`test_main.c`を
ちゃんと扱う方法もわからず、とりあえずダミーのmain関数のファイルを作ってそちらを使ってビルドすることにした。(もちろんlinux環境上では
cmakeは使わずにビルドするので特に問題ない。) やはりcmakeは高度すぎてよくわからない。

#### [配列定義で[]の結合が逆だったので修正](https://github.com/crhg/9ninecc/commit/4e2a6836a282eb285dfa12c628cfd5cb43fe6f3e)

2次元配列のテストを追加したらさっそくバグっていたので修正。

左再帰を適当にループ展開したら結合が逆になっていた。ループだと考えにくいので左再帰がなくなるように文法を変換して再帰下降で書き直した。

#### [Linux上でなければmakeをdockerで行う](https://github.com/crhg/9ninecc/commit/9ee9545348dc057304b231a52f32d1736a63ee38)

mac上のCLionで開発するならいちいち確認のために開発サーバにログインするのも面倒になったのでdockerでテストができるようにした。

#### [clang-formatが必要だった](https://github.com/crhg/9ninecc/commit/3c157cf15ee924fc50ca9c26a4923fb280667af9)

clang-formatを使っているのにインストールを忘れていた。

#### [test_sourceのmake clean](https://github.com/crhg/9ninecc/commit/426420586b77fee6d0d0c4819009563c79daf2fc)

test_sourceディレクトリもmake cleanで掃除されるようにした。

#### [strprintfのテスト](https://github.com/crhg/9ninecc/commit/8e89b53932599e05407b1313e1a38529969d72c3)

エンバグして煮詰り、strprintfがあやしいような気がしたので`-test`のテストを久しぶりに追加。
フォーマット文字列の書き方が悪かっただけでstrprintfは無罪だった...
バグが見つかったから良しとする。

#### [gen_test_mail.plで@XXを認識するコメントは行頭//+空白で開始するように変更](https://github.com/crhg/9ninecc/commit/e1721b579a87ae6c5d59298002da7556ddc6882c)

テストをコメントアウトしたつもりがコードはコメントアウトされていても`@try_ret`などのコメントに書く指示文字列は拾われてしまうのでおかしなmainが生成される問題。
コメントアウトを2重にやると`////`と空白が空かないのを利用して行頭の`//`のあとにさらに空白がある場合のみを対象にする。

#### [va_endはあった方がいい?](https://github.com/crhg/9ninecc/commit/2a4c3b8b4c235cddd9f1ac350737dd44ef03d488)

どうなんだろうか??

#### [スカラー変数の数値での初期化](https://github.com/crhg/9ninecc/commit/e2b3136ecffd5a2909902629a5dd383685a43c87)

以下のパターンの初期化ができるようになった。

```C
int x = 42;
```

## 2019-06-07

#### [.gitignoreにfailed-test.sを追加](https://github.com/crhg/9ninecc/commit/055a65afb3ee5f7a965980207ed99fac8316dfb3)

#### [verror_atでエラーメッセージを出力する前に標準出力をfflushする](https://github.com/crhg/9ninecc/commit/d8d415777d97966a64be04784bd14b1e4a7b2ece)

9nineccを出力をファイルにリダイレクトしないで動作させたとき、stdoutに出力されるコードがバッファに溜まっていてstderrに出力されるエラーメッセージが先に出ることがありわかりにくいので、エラーメッセージの出力前にstdoutをfflushするようにした。

#### [文字列リテラルによるポインタ型のグローバル変数の初期化](https://github.com/crhg/9ninecc/commit/9b4d22e105e796d308303a283b3bb414e68927a2)

```C
char *s = "hoge";
```

#### [配列変数名によるポインタ値での初期化と加減算](https://github.com/crhg/9ninecc/commit/1eb129743d3e79ee12b9433544cf2fc9dd21b5ea)

コミット忘れで2つが混じった。

```C
int a[10];
int *p = a;
```

と

```C
char *s = "hoge" + 1;
```

加減算以外の演算子は面倒なので積み残し。

比較演算子とポインタがからむと面倒そうであった。gccで試したところ

```C
int x;
int f1 = &x == 0; // 0との比較はOK。定数1になる
int f2 = &x == 1; // これはエラー
int f3 = &x > 0; // 0との比較はできる。定数1になる
int f4 = &x > 1; // これはエラー
int f5 = &x - 1 > 0; // これはOK。定数1
```

これらから考えるに

* 変数からとったポインタとそれに加減算したものと定数0は比較できて、ポインタ型は0ではない値とみなされる。
* ポインタと0以外の数は比較できない

となっているようだが?

#### [グローバル変数初期化式で'&'演算子が使えるようにした](https://github.com/crhg/9ninecc/commit/c08573fb5f0ca4c7ca57df16d2cad03d03682a69)

単項`&`演算子対応。内部で使われるので単項`*`にも対応。

```C
int x[100];
int *p = &x[10];
```

#### [エラー表示関数を整理](https://github.com/crhg/9ninecc/commit/e0d97928baad86dd1a1a72a21c96366e2dba688d)

警告メッセージを出すけどプログラム終了はしない奴が欲しかったので一式追加。

#### [エラー監修整理でwarnの修正漏れ](https://github.com/crhg/9ninecc/commit/ea9955cb50e01bd334ccaf75d28d5fb353080b73)

warnだけ9ninecc.hの中で記述箇所が離れていたので修正漏れ。

#### [グローバル変数の初期化リストによる初期化](https://github.com/crhg/9ninecc/commit/1a0e19eed5f432a5805094681cd9a7c28cd25526)

配列の初期化に対応。
