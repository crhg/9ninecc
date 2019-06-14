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

いつの時点でスタックが16バイト境界を指しているべきなのかよくわからなくていろいろ調べる。~~関数が呼ばれたときに16バイト境界にあればいいらしい。
~~ call命令の直前に16バイト境界にあればいい。

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

#### [初期値対応: 既存のテストはパス](https://github.com/crhg/9ninecc/commit/5123492ab5cfb9552700b9966bfdf3baa4414448)

初期化を使わない既存のテストコードが通るようになったのでいったんコミット。

ためしにCLionを使ってみることにしたのでcmakeの設定なども追加。

Mac上のCLionなので9nineccの出力するコードはそのままでは実行できないから、CLion上でテストコードをコンパイルできるようになるまではMac上で、
結果のコードの実行確認はlinuxにコピーして行うという感じの作業にした。CLionはcmakeなのでMakefileは見ないからちょうどいい。

#### [インスペクターに怒られるところを修正](https://github.com/crhg/9ninecc/commit/540c91dd7a910128459320acd45ecc96bda15183)

CLionのインスペクターがあやしいところを指摘してくれるので徐々に修正。

#### [重複した宣言を削除](https://github.com/crhg/9ninecc/commit/38c2127952b90c8ae80936479ebf2321939ab06d)

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

グローバル変数の初期値は最終的にアセンブラに落とした段階でラベル+整数になってなければいけないということなので、
ラベルと整数の組を結果とする式の評価関数を作ってみた。

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

#### [Merge pull request #1 from crhg/initializer](https://github.com/crhg/9ninecc/commit/04f1acf99febc3c521400489a115b3e57926eb77)

グローバル変数の初期化対応は一通り終わったのでmasterにマージ。

## 2019-06-08

今度はローカル変数の初期化を行う

### [type_specの先読み対応](https://github.com/crhg/9ninecc/commit/deb05381fc0fb5e6c102eb4158c208a0e08740c3)

いままでは`type_spec`を呼び出す前に次のトークンが`int`か`char`であることを調べていたが、
`type_spec`が`NULL`を返すようにして先に調べなくて良いように変更。

### [変数定義の後半部分を関数にくくり出す](https://github.com/crhg/9ninecc/commit/2f0f71a263ce6bace7cd38ae763f16ac8a415a85)

グローバル変数の初期化対応で作成した部分だがローカル変数の初期化も同じような処理が必要なので共通で使えそうな
部分を関数でくくり出した。

## 2019-06-09

### [DeclInitのtypedefを9ninecc.hに移動](https://github.com/crhg/9ninecc/commit/1cded0f2d18af045fde7e98e0211e0ae2adb6127)

### [ローカル変数宣言をdeclaratorを使って書き直し](https://github.com/crhg/9ninecc/commit/b5bc3a4f50d35835b77b7b30e2e9e73438f95781)

ローカル変数宣言を先にくくりだしておいた関数を使って書き直し。

### [スカラー型のローカル変数の初期化](https://github.com/crhg/9ninecc/commit/ff05aa7de8ec45ec7297e13c3a3cf31389a62f71)

いままでなにもコード生成していなかった`ND_LOCAL_VAR`ノードに`Declarator`のベクタを追加して
初期値設定のコードを生成するようにした。

### [配列型のローカル変数の初期化](https://github.com/crhg/9ninecc/commit/4abeb83057f85955f2e869ba49867969b6c856a3)

やっていることはグローバル変数のときと似ていて、以下のように処理をローカル変数用に置き換えた感じ。

* 式の値を評価するかわりに計算するコードを生成
* それをアセンブラの定数にするのではなく決められたスタックフレームのオフセットに保存するコードを生成
* 初期値が足りない部分をゼロで埋めるコードを生成

ゼロ埋めはいい方法が思いつかなかったのでとりあえず`bzero`コールで。

### [宣言のときTypeにトークンを持たせる(エラー表示用)](https://github.com/crhg/9ninecc/commit/0fa969efae43625c22fa0c3849af9cb9bb47d975)

型のまわりでエラーがでるとソースのどこに対応するかわかりにくかったので、エラー表示用にトークンを保持するようにした。

### [サイズを省略した配列の初期化](https://github.com/crhg/9ninecc/commit/e52e783b3ea2e06eb6c19c43dbfb8ae7066fadfe)

とりあえず書いてはみたがサイズ省略周りはまだちゃんと理解できてない感じ。

### [配列の長さと配列のバイト数が混乱しがちなので長さはlen,バイト数はsizeにする。](https://github.com/crhg/9ninecc/commit/d5873a1d78b3de61b0ada68f4f1ef17d913f5f5e)

配列の長さ(=要素数)の意味でサイズを使うとデータのバイト数のサイズと混乱しがちなので、配列の長さはlenを使うように統一。

### [不要コードの整理](https://github.com/crhg/9ninecc/commit/3e4d7cb52a0431bddf89ee94b9991dc287b82e99)

コミットログ間違えてるのに気がついたが直し方がわからない。

### [CLionで補完が効くようにするためTypeのtyのenum定義を外に出す](https://github.com/crhg/9ninecc/commit/644818fe1b28833b3917817d28dfab3aa85d0e7e)

### [Inspection Scopeの設定](https://github.com/crhg/9ninecc/commit/40358ee932ee64660e53e01b0a360d2abcd40568)

これもコミットログが変。

CLionのAnalyzeを全ソースにするとtest_source以下のコードがひっかかりまくるので、コンパイラ本体の
コードのみのスコープを定義した。

## 2019-06-10

### [TODOリストを追加](https://github.com/crhg/9ninecc/commit/f40efb37e1e0ae1d4d27f9f39d71363d9bcf2a6a)

いままではテキストに従えば良かったがここから先は手探りなので、TODOリストを作成して見ながら行うようにする。

### [現在位置のトークンのエラーを楽に書くためerror_at_here, warn_at_hereを作って置き換え](https://github.com/crhg/9ninecc/commit/c435d6e353dc038c1d0e31ef9f6981574807169f)

`error_at(TOKEN(pos)->input,...)`はTOKENのあとを手書きするのが面倒になってきたので`error_at_here(...)`等を作成。
これだとコード補完できるので楽。

### [空文](https://github.com/crhg/9ninecc/commit/e4a2a55645f58fb65b323ad3af6aef1429d60fd0)

TODO作ったときに空文は一瞬でできそうだなと思ったので作成。

### [round_up関数を作ってローカル変数のオフセット計算を整理](https://github.com/crhg/9ninecc/commit/69592b25f20164775a9c40e0dbc2047c21b6156f)

構造体に手を付けるに当たって、ローカル変数のオフセット割り当てで気になっていた煩雑なコードを
`round_up`(切り上げ)関数を作って整理した。

### [構造体型変数の定義](https://github.com/crhg/9ninecc/commit/c2aa2aa4378a325c99dabb265482414899f4f623)

まず構造体型をTypeに追加。フィールドの型とオフセットの組をフィールド名でひけるMapをもつような感じにした。
構造体の全体のサイズやalignmentも保持。

オフセットの割り当てはとりあえず出てきた順に頭から、すぐ後に付けるとアラインメントが合わないときは隙間があく
感じで実装したが、なにか規定がありそうな気がする?

全体のアラインメントは要素のアラインメントの最大値。

全体のサイズは最後のフィールドが収まるサイズを全体のアラインメントの倍数に切り上げ。

### [->を実装](https://github.com/crhg/9ninecc/commit/a05bfa7c11355eba2ddea043af95be4d803ba920)

`->`を先にしたのはどうせフィールドアクセスは構造体の先頭番地にフィールドのオフセット足してみたいな感じに
なるので、構造体のポインタ起点になる`->`の方が楽そうだったから。

あと構造体の`&`も変数の先頭番地を求めるだけなのでほぼコード書かなくて良さそうというのもある。

さらに`x->y`はパース時にこっそり構文木が`*(&(x->y))`になるようにした。こうすると`x->y`に関するコード生成は
左辺値のときだけですむので楽。

### [TODO追加](https://github.com/crhg/9ninecc/commit/9ce3638db567b72d933914f0172f529625a9e506)

### [.の実装](https://github.com/crhg/9ninecc/commit/3c4d1abc040c2ba977c90e811139ebc336f8d8ec)

`x.y`をパース時に`*(&((&x)->y))`になるようにした。コード生成の追加がいらなくて楽。

### [new_node_arrowがわかりにくい気がしたのでコメント追加](https://github.com/crhg/9ninecc/commit/37b06e799d30bef09f58e40e5cf0eeb0931799d8)

忘れそうなのでコメントに書いておく。

## 2019-06-11

### [ヒストリ追記](https://github.com/crhg/9ninecc/commit/fc709d630c26d9b1553ef421aac0012ecd778d01)

### [型のincompleteフラグ](https://github.com/crhg/9ninecc/commit/fc31985e4c1c585f285a4d1ce587fee497b42e2d)

名前つきstructの実装に先立ち、長さが決まっていない配列型のincomplete_lenフラグを中身がまだ決まっていない名前だけのstruct型にも使えるようにincompleteと名前を変更。

### [TODO追加](https://github.com/crhg/9ninecc/commit/81a2e94eff8a44acbbf2eb543518b2ceb767e648)

### [名前つきのstruct](https://github.com/crhg/9ninecc/commit/45f11d4a1b32fccbb06f5354480bafb212631256)

以下のように実装。

* 構造体の名前と型の対応表をグローバルなMapとして用意。

* 構造体の中身が定義されたらimcompleteが0(false)になる。

## 2019-06-12

### [TODO追加](https://github.com/crhg/9ninecc/commit/580d78f28a1c5d71aa164d39df43e7e56f9ca95f)

### [識別子に英大文字が使えない問題が見つかったので修正](https://github.com/crhg/9ninecc/commit/479a9356bbe592dd496f797f451b24fa1ab3517f)

すこしテストを追加したら識別子に英大文字が使えない問題が発覚した。修正。

### [変数を定義しないstructだけの宣言ができるようにする](https://github.com/crhg/9ninecc/commit/cf4f7830187ec182875f047a4ff54b472d21f9bf)

<type_spec>のあとに<declarator>ではなく';'が来ても受け入れるようにした。

本当はintのようになにも宣言しない<type_spec>の場合は受け入れてはいけないのだけどそれは未実装。

### [TODO更新](https://github.com/crhg/9ninecc/commit/ab3c04451cb6203c1a50c780cc05b8953adfe3a2)

### [不要なコードを整理](https://github.com/crhg/9ninecc/commit/5cae65293a0413fbb12bb7dfe2ab97ad93345d80)

### [関数呼び出しのスタック調整の説明を読み間違えていたので修正](https://github.com/crhg/9ninecc/commit/bfaef588bbd80603fcce4cd81c11b239c71c637a)

よくテキストを読んだらcall命令の実行前にスタックポインタが16バイト境界にあることとはっきり書いてあったので修正。

### [TODO更新](https://github.com/crhg/9ninecc/commit/e0132c01d8ea7a50545ef18388118aac5abedf1f)

### [CLionのデバッガでの表示をみやすくするため、1文字のトークンやノードにも名前を付ける](https://github.com/crhg/9ninecc/commit/fa04c93020fe113fc544951077f78e781c1cb57a)

トークンやノードの種類を表すのに1文字の記号については文字コードをそのまま使うようになっていたけど、CLionで以下の問題があるので全部に名前を付けた。

* デバッガでは文字コードの数値で表示されるのでどの文字かわかりにくい。(enumで名前がついていれば名前で表示してくれる)

* 文字定数だとFind Usageが使えない(文字列でのサーチになる)

### [記号と名前の対照表からenum宣言の中身を作るツール](https://github.com/crhg/9ninecc/commit/f40f3356263681953d66172b46d75caba1d3df59)

### [使われなくなったコードを削除](https://github.com/crhg/9ninecc/commit/a58dac1506ff7ea7ff817c287ca0355a471c5ab9)

## 2019-06-13

### [union](https://github.com/crhg/9ninecc/commit/6e3fddc962329b00220b0193fb590c4445915177)

structとあまり違わないのですぐにできた。違いは以下ぐらい。

* オフセットは常に0
* 全体のサイズの計算方法が異なる(フィールドのサイズのmaxを全体のアラインメントの倍数に切り上げ)

しかし、何故かバグって悩む。構文木を表示するデバッグ出力などを作成した。
結局どこかでノード自体の型とノードが指すノードの型を取り違えてたのが原因だった。

木をちゃんと木らしくインデントつけて表示するのは面倒だったので、LispのS式風の出力にした。(長いときはemacsに貼って整形する前提)

デバッグ出力の部分は分けてコミットすればよかったと後で思う。

### [TODO更新](https://github.com/crhg/9ninecc/commit/89b6d0f84b4c86be86fa51adad90645ec5a0a623)

### [structおよびunionのローカル変数の初期化](https://github.com/crhg/9ninecc/commit/7df798b705e4c3ad543c8f5379d3d1d084f27c37)

struct,union用の初期化リストを読み込んで初期化されるフィールドは再帰的に初期化コード生成、初期値が割り当たってないフィールドは0埋めという感じで書いた。

### [structおよびunionのグローバル変数の初期化](https://github.com/crhg/9ninecc/commit/35674006c7ecf8f736701e4954cb4238bddf06a7)

グローバル変数も同じような感じ。0埋めが.zeroを生成するだけでいいのでむしろ楽。

#### 謎の`.align 16`

実装前にgccの生成するコードを見ていたら、

```C
typedef struct A {
    int x;
    char s[20];
} A;

A a = { .x = 10, .s = "hoge"};
```

が

```asm
        .globl  a
        .data
        .align 16
        .type   a, @object
        .size   a, 24
a:
        .long   10
        .string "hoge"
        .zero   15
```

となっていて、どこから`.align 16`が出てくるのか謎だった。

https://software.intel.com/sites/default/files/article/402129/mpx-linux64-abi.pdf の 3.1.2
Machine Interface Processor Architecture Data Representation に

> Aggregates and Unions
> Structures and unions assume the alignment of their most strictly aligned compo- nent. Each member is assigned to the lowest available offset with the appropriate alignment. The size of any object is always a multiple of the object‘s alignment.
> An array uses the same alignment as its elements, except that a local or global array variable of length at least 16 bytes or a C99 variable-length array variable always has alignment of at least 16 bytes.4
> Structure and union objects can require padding to meet size and alignment constraints. The contents of any padding is undefined.

とあり、これを単独の構造体変数にも16バイトを以上なら適用しているっぽい。

この規定の理由は注4に書いてあって、

> The alignment requirement allows the use of SSE instructions when operating on the array. The compiler cannot in general calculate the size of a variable-length array (VLA), but it is ex- pected that most VLAs will require at least 16 bytes, so it is logical to mandate that VLAs have at least a 16-byte alignment.

SSE使いたいからのようです。(SSEの128bitレジスタは16バイト境界にあるデータしか読み書きできない)