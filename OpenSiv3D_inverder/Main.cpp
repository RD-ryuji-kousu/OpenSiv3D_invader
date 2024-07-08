# include <Siv3D.hpp> // Siv3D v0.6.14

//スタート画面
class Start {
public:
	void View() {
		font(first).draw(Arg::center(400, 200));
		font(second).draw(Arg::center(400, 400));
	}
private:
	const Font font{ 60 };
	String first = U"Start press 'F'";
	String second = U"Choose second move?\n Press 'S'";
};

//ゲームオーバー画面
class Over {
public:
	/*
	@param[in] text1	条件に則したテキストを受け取る
	*/
	void View(const String& text1) {
		title(text1).draw(Arg::center(400, 100), Palette::Red);
		font(restart).draw(Arg::center(400, 400));
		font(quit).draw(Arg::center(400, 500));
	}
private:
	const Font title{ 100 };
	const Font font{ 60 };
	String restart = U"Restart Press'R'";
	String quit = U"Quit Press 'Q'";
};
class Walls;
class Enemies;
//自機の初期位置
const Vec2 PL_START_POS{ 400, 550 };

//自機の管理クラス
class Player {
private:
	int life, ret;
	Texture jiki{ U"inverder_png/jiki.png" };
	Vec2 plpos;
	Size txsz;
	double dx;
public:
	/// @brief コンストラクタ
	/// @param _life ライフ
	/// @param _plpos 自機の位置
	/// @param _txsz 描画されるテクスチャのサイズ
	Player(int _life, Vec2 _plpos, Size _txsz) :life(_life), plpos(_plpos), dx(300), txsz(_txsz), ret(120){
		
	}
	/// @brief 自機の操作
	/// @param[in,out] flag trueの時キーボードの　<-, A, ->, Dなどで操作する。falseの時120f後再配置
	void move(bool& flag) {
		if (flag == true) {
			//左移動
			if (KeyLeft.pressed() || KeyA.pressed()) {
				plpos.x -= dx * Scene::DeltaTime();
				//自機の左端が画面外からでないようにする
				if ((plpos.x - (txsz.x / 2)) <= 0) {
					plpos.x = 0 + txsz.x / 2;
				}
			}
			//右移動
			if (KeyRight.pressed() || KeyD.pressed()) {
				plpos.x += dx * Scene::DeltaTime();
				//自機の右端が画面外からでないようにする
				if ((plpos.x + (txsz.x / 2)) >= 800) {
					plpos.x = 800 - txsz.x / 2;
				}
			}
		}
		else
		{
			if (ret != 0) {
				plpos = { 820, 606 };
				ret--;
			}
			else {
				plpos = PL_START_POS;
				ret = 120;
				flag = true;
			}
		}
	}
	/// @brief 自機の描画
	void Draw() {
		jiki.resized(txsz).drawAt(plpos);
	}
	/// @brief 自機の位置を返す
	/// @return 自機の位置
	const Vec2& jikipos()const {
		return plpos;
	}
	Vec2& jikipos() {
		return plpos;
	}
	/// @brief 自機のサイズを返す
	/// @return 自機のサイズ
	const Size& jikisz()const {
		return txsz;
	}
	Size& jikisz(){
		return txsz;
	}
	
};
/// @brief 自機から発射される弾を管理する;
class Shot {
private:
	Size sz;
	bool bullet_max;
	Texture beam{ U"inverder_png/beam1.png" };
	Vec2 bpos, beamV;
	ColorF beamc;
public:
	/// @brief コンストラクタ
	/// @param[in] _sz 描画されるテクスチャのサイズ
	Shot(Size _sz) :bullet_max(false), sz(_sz), bpos(Vec2(-40, -40)) {
		beamc = Palette::Seagreen;
		beamV = { 0,400 };
	}
	/// @brief 描画
	void Draw() {
		beam.resized(sz).drawAt(bpos, beamc);
	}
	void calc_shot(const Player& pl, Enemies& enemy, Walls& wall,bool& anim, Vec2& apos);
	/// @brief 発射物の位置を返す
	/// @return 弾の位置
	const Vec2& Get_bpos()const {
		return bpos;
	}
	const Size& Get_sz()const {
		return sz;
	}
	const Size& Get_txsz()const {
		return beam.size();
	}

};

/// @brief 被弾時アニメーション
class HitAnim {
private:
	int fpsc_e, fpsc_p;
	Size sz;
	Texture explode, anim;
	Vec2 apos;
	ColorF color;
public:
	/// @brief コンストラクタ
	/// @param _sz テクスチャのサイズ
	HitAnim(Size _sz) : sz(_sz), explode(U"inverder_png/delete.png"),
		anim(U"inverder_png/delete_anim.png"), apos(-40, -40), color(Palette::Azure), fpsc_e(0), fpsc_p(0) {}
	/// @brief 敵被弾時アニメーション描画
	/// @param[in] pos 被弾した敵座標
	/// @param[in, out] flag アニメーションをtrueの時描画
	void DrawE(Vec2 pos, bool& flag) {
		if (flag == true) {
			if (fpsc_e >= 0 && fpsc_e <= 7) {
				explode.resized(sz).drawAt(pos, color);
			}
			else {
				anim.resized(sz).drawAt(pos, color);
				if (fpsc_e == 15) {
					pos = { -40, 40 };
					fpsc_e = 0;
					flag = false;
				}
			}
			fpsc_e++;
		}
	}
	void DrawP(Vec2 pos, bool& flag) {
		if (flag == true) {
			if (fpsc_p >= 0 && fpsc_p <= 60) {
				explode.resized(sz).drawAt(pos, color);
			}
			else {
				anim.resized(sz).drawAt(pos, color);
				if (fpsc_p == 120) {
					pos = { -40, 40 };
					fpsc_p = 0;
					flag = false;
				}
			}
			fpsc_p++;
		}
	}
};

/// @brief 敵一体の描画情報クラス
class Enemy : public Texture{
public:
	Size txsz;
	ColorF color;
	Vec2 epos;
	Texture anim;
	int score;
	/// @brief コンストラクタ
	/// @param[in] _enemy_pos 敵のテクスチャ
	/// @param[in] _anim アニメーション
	/// @param[in] _color 色
	/// @param[in] _txsz テクスチャサイズ
	/// @param[in] _epos 敵一体の座標
	/// @param[in] _score 敵一体から獲得できるスコア
	Enemy(const Texture& _enemy_pos, const Texture& _anim,ColorF _color, Size _txsz, Vec2 _epos, int _score) :
		Texture(_enemy_pos), anim(_anim), color(_color), txsz(_txsz), epos(_epos), score(_score){		
	}
};

class Enemies {
private:
	Array<Enemy> enemies;
	int score;
	double dx = 20;
	double move_max;
	int move_y;
	int part = 0;
	Font font;
public:
	//コンストラクタ
	///@param[in]テクスチャのサイズ
	Enemies(const Size& txsz):move_max(100), move_y(0), score(0), font(20) {
		ColorF enmyc = { 0, 0, 0 };
		FilePathView path = U".", anim = U".";
		int point = 0;
		for (int y = 0; y < 5; y++) {
			for (int x = 0; x < 11; x++) {
				switch (y)
				{
				
				case 0:
					enmyc = { Palette::Magenta };
					path = U"inverder_png/enemy1.png";
					anim = U"inverder_png/enemy1_anim.png";
					point = 10;
					break;
				case 1:
					enmyc = { Palette::Magenta };
					path = U"inverder_png/enemy1.png";
					anim = U"inverder_png/enemy1_anim.png";
					point = 10;
					break;
				case 2:
					enmyc = { Palette::Aqua };
					path = U"inverder_png/enemy2.png";
					anim = U"inverder_png/enemy2_anim.png";
					point = 20;
					break;
				case 3:
					enmyc = { Palette::Aqua };
					path = U"inverder_png/enemy2.png";
					anim = U"inverder_png/enemy2_anim.png";
					point = 20;
					break;
				case 4:
					enmyc = { Palette::Greenyellow };
					path = U"inverder_png/enemy3.png";
					anim = U"inverder_png/enemy3_anim.png";
					point = 30;
					break;
				default:
					break;
				}
				enemies << Enemy(Texture{ path }, Texture{ anim }, enmyc,
					txsz, Vec2(150 + x * txsz.x + x * 20, 300 - (y * txsz.y) - (y * 20)), point);
			}

		}
	}
	/// @brief 敵の描画
	void Draw() {
		for (const auto& enemd : enemies ) {
			//partを60で割った余りが偶数か奇数かで描画パターンを変える
			if (IsEven(part / 60)) {
				enemd.resized(enemd.txsz).drawAt(enemd.epos, enemd.color);
			}
			else {
				enemd.anim.resized(enemd.txsz).drawAt(enemd.epos, enemd.color);
			}
		}
		//スコアの表示
		font(U"Score\n", score).draw(0, 0);
		part++;
	}
	/// @brief 敵が被弾したときの処理
	/// @param[in] bpos 自機の弾の位置
	/// @param[in] shot_sz 自機の弾のサイズ 
	/// @param[in, out] anim 被弾したとき描画するためのtrue,falseを返す, 被弾したときtrue
	/// @param apos 
	/// @return 
	bool hit(const Vec2& bpos, const Size& shot_sz, bool& anim, Vec2& apos) {

		for (auto it = enemies.begin(); it != enemies.end();) {
 			if ( it->epos.y - (it->txsz.y/2) <= bpos.y - shot_sz.y/2 && (it->epos.y + it->txsz.y / 2) >= bpos.y - shot_sz.y/2
				|| it->epos.y - (it->txsz.y / 2) <= bpos.y + shot_sz.y / 2 && (it->epos.y + it->txsz.y / 2) >= bpos.y + shot_sz.y / 2){
				if (it->epos.x - (it->txsz.x / 2) <= bpos.x - shot_sz.x / 2 &&
					it->epos.x + (it->txsz.x / 2) >= bpos.x - shot_sz.x / 2 ||
					it->epos.x - (it->txsz.x / 2) <= bpos.x + shot_sz.x / 2 &&
					it->epos.x + (it->txsz.x / 2) >= bpos.x + shot_sz.x / 2) {
					anim = true;
					apos = { it->epos.x, it->epos.y };
					score += it->score;
					it = enemies.erase(it);
					return true;
				}
				
			}
			it++;
		}
		return false;
	}
	void move() {
		move_max -= abs(dx * Scene::DeltaTime());
		for (auto it = enemies.begin(); it != enemies.end();it++) {
			
			if (move_max <= 0&&move_y != 30) {
				it->epos.y += it->txsz.y / 30;
			}
			else {
				it->epos.x += dx * Scene::DeltaTime();
			}
		}

		if (move_max <= 0 && move_y != 30) {
			move_y++;
		}
		if (move_max <= 0 && move_y == 30) {
			dx *= -1;
			move_max = 200;
			move_y = 0;
		}

	}
	Vec2 rand_epos()const {
		double rx = Sample(enemies).epos.x, dy = 0;
		int i = 0, index = 0;
		for (auto it = enemies.begin(); it != enemies.end();it++, i++) {
			if (it->epos.x == rx) {
				if (it->epos.y > dy) {
					dy = it->epos.y;
				}
				else if (index == 0 && dy == 0) {
					index = i;
				}

			}
		}
		return Vec2(rx, dy);
	}
};

class Wall : public DynamicTexture{
public:
	Size sz;
	ColorF color;
	Vec2 wpos;
	Image wall;
	int index;
 	Wall(const Image& _wall ,Size _sz, Vec2 _wpos, int i) :DynamicTexture(_wall), sz(_sz), wpos(_wpos),
		color(Palette::Red), wall(_wall), index(i) {
		
	}
	double Break_wall(const Vec2& bpos, const SizeF& bsz) {
		int count = 0;
		int x0 = (int)bpos.x;
		int x1 = (int)bpos.x + bsz.x;
		int y0 = (int)bpos.y;
		int y1 = (int)bpos.y + bsz.y;
		if (x0 < 0) {
			x0 = 0;
		}
		if (x1 > wall.width()) {
			x1 = wall.width();
		}
		if (y0 < 0) {
			y0 = 0;
		}
		if (y1 > wall.height()) {
			y1 = wall.height();
		}
		for (int y = y0; y < y1; y++) {
			for (int x = x0; x < x1; x++) {
				if (wall[y][x].a != 0) {
					count++;
					wall[y][x].a = 0;
				}
			}
		}
		if (x1 == x0 || y1 == y0)return 0.0;
		return (double)count / ((x1 - x0) * (y1 - y0));
	}
};

const int WALL_FIRST_POS = 150;
const int WALL_SPACE = 65;

class Walls {
private:
	Array<Wall> walls;
public:
	Walls(Size sz) {
		for (int i = 0; i < 4; i++) {
			walls << Wall(Image{ U"inverder_png/zangou.png" }, sz, Vec2(150 + (i * sz.x) + (i * 65), 480), i);
		}
	}
	void Draw() {
		for (const auto& walld : walls) {
			walld.resized(walld.sz).drawAt(walld.wpos, walld.color);
		}
	}
	/// @brief ブロックの命中処理
	/// @param bpos[in] 弾の中心座標 
	/// @param[in] sz 描画サイズ
	/// @param[in] txsz テクスチャサイズ
	/// @return 命中したらtrue
	bool hit(const Vec2& bpos, const Size& sz) {
		double count = 0, y0, y1, x0, x1;
		double sx0 = bpos.x - sz.x / 2, sx1 = bpos.x + sz.x / 2, sy0 = bpos.y - sz.y / 2, sy1 = bpos.y + sz.y / 2;
		double iszx, iszy;
		Vec2 ibpos;
		SizeF tmpsz;
		for (auto it = walls.begin(); it != walls.end(); it++) {
			y0 = it->wpos.y - it->sz.y / 2;
			y1 = it->wpos.y + it->sz.y / 2;
			x0 = it->wpos.x - it->sz.x / 2;
			x1 = it->wpos.x + it->sz.x / 2;
			iszx = (double)it->wall.width() / it->sz.x;
			iszy = (double)it->wall.height() / it->sz.y;
			tmpsz = { sz.x * iszx, sz.y * iszy };
			if (y0 < sy1 && sy0 < y1) {
				if ( x0 < sx1 && sx0 < x1) {
					ibpos = { (sx0 - x0) * iszx, (sy0 - y0) * iszy };
					
					count = it->Break_wall(ibpos, tmpsz);
					it->fill(it->wall);
					Print << count;
					if (count >= 0.15) {
						
						return true;
					}
				}
			}
		}
		return false;
	}
};


class Eshot {
private:
	Texture shot{U"inverder_png/enemybomb.png"};
	Vec2 bpos, shotV;
	ColorF color;
	Size sz;
	bool bomb_flag;
	int rate;
public:
	Eshot(Size _sz) : bpos(Vec2(-80, -80)), shotV(Vec2(0, 200)),
		color(Palette::Yellow), sz(_sz), bomb_flag(false), rate(Random<int>(30, 60)){
	}
	void Draw() {
		shot.resized(sz).drawAt(bpos, color);
	}
	void calc_eshot(const Enemies& enemy, const Player& pl, Walls& wall,int& life, bool& anim, Vec2& apos, bool& flag) {
		if (bomb_flag == false && rate == 0) {
			bpos = enemy.rand_epos();
			bomb_flag = true;
		}
		else {
			bpos += shotV * Scene::DeltaTime();
			if (bpos.y >= 600) {
				bomb_flag = false;
				bpos = { -40, -40 };
				rate = Random<int>(30, 60);
			}
			if (pl.jikipos().y - pl.jikisz().y/2 <= bpos.y + sz.y / 2) {
				if (pl.jikipos().x - pl.jikisz().x <= bpos.x - sz.x &&
					pl.jikipos().x + pl.jikisz().x >= bpos.x - sz.x ||
					pl.jikipos().x - pl.jikisz().x <= bpos.x + sz.x &&
					pl.jikipos().x + pl.jikisz().x >= bpos.x + sz.x) {
					life--;
					anim = true;
					flag = false;
					apos = pl.jikipos();
					bomb_flag = false;
					bpos = { -40, -40 };
					rate = Random<int>(30, 60);
				}
			}
			if (wall.hit(bpos, sz) == true) {
				bomb_flag = false;
				Print << U"!!!";
				bpos = { -40, -40 };
				rate = Random<int>(30, 60);
			}
		}
		rate--;
	}

};



void Shot::calc_shot(const Player& pl, Enemies& enemy, Walls& wall,bool& anim, Vec2& apos) {
	if (bullet_max == false ) {
		if (KeySpace.pressed()) {
			bpos = pl.jikipos();
			bullet_max = true;
		}
	}
	if (bullet_max == true) {
		bpos -= beamV * Scene::DeltaTime();
		if (bpos.y < 0) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
		if (enemy.hit(bpos, sz, anim, apos) == true) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
		if (wall.hit(bpos, sz) == true) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
	}
	
}

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ Palette::Black });

	Size txsz_chara{ 30, 30 };
	Size txsz_bullet{ 15, 20 };
	Size txsz_obj{ 100,100 };

	Rect debug_rect{ Arg::center(400, 300), 30, 30 };
	int life = 3;
	Player pl(life, PL_START_POS, txsz_chara);
	Enemies enemy(txsz_chara);
	Walls wall(txsz_obj);
	Shot shot(txsz_bullet);
	Eshot bomb(txsz_bullet);
	HitAnim anim(txsz_chara);
	bool exflag_e = false, exflag_p = false, move_flag = true;
	Vec2 apos_e{ -40,-40 }, apos_p{ -40, -40 };
	Font font_life{ 20 };

	while (System::Update())
	{
		wall.Draw();
		pl.Draw();
		enemy.Draw();
		pl.move(move_flag);
		enemy.move();
		shot.calc_shot(pl, enemy, wall,exflag_e, apos_e);
		shot.Draw();
		bomb.calc_eshot(enemy, pl, wall,life, exflag_p, apos_p, move_flag);
		bomb.Draw();
		anim.DrawE(apos_e, exflag_e);
		anim.DrawP(apos_p, exflag_p);
		font_life(U"Life", life).draw(Arg::bottomLeft(0, 600));
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
