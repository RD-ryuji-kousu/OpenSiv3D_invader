# include <Siv3D.hpp> // Siv3D v0.6.14
#define FRAME_TIME (1.0/60) //Scene::DeltaTime()

//スタート画面
class Start {
public:
	void View() {
		font(first).draw(Arg::center(400, 200));
		//font(second).draw(Arg::center(400, 400));
	}
private:
	const Font font{ 60 };
	String first = U"Start press 'F'";
	//String second = U"Choose second move?\n Press 'S'";
};

//ゲームオーバー画面
class Over {
public:
	/*
	@param[in] text1	条件に則したテキストを受け取る
	*/
	void View() {
		title(text1).draw(Arg::center(400, 100), Palette::Red);
		font(restart).draw(Arg::center(400, 400));
		font(quit).draw(Arg::center(400, 500));
	}
private:
	const Font title{ 100 };
	const Font font{ 60 };
	String text1 = U"Missed!!";
	String restart = U"Restart Press'R'";
	String quit = U"Quit Press 'Q'";
};
class Walls;
class Enemies;
class Bonus;
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
	Player(int _life, const Vec2& _plpos, const Size& _txsz) :life(_life), plpos(_plpos), dx(300), txsz(_txsz), ret(120){
		
	}
	/// @brief 自機の操作
	/// @param[in,out] flag trueの時キーボードの　<-, A, ->, Dなどで操作する。falseの時120f後再配置
	void move(bool& flag) {
		if (flag == true) {
			//左移動
			if (KeyLeft.pressed() || KeyA.pressed()) {
				plpos.x -= dx * FRAME_TIME;
				//自機の左端が画面外からでないようにする
				if ((plpos.x - (txsz.x / 2)) <= 0) {
					plpos.x = 0 + txsz.x / 2;
				}
			}
			//右移動
			if (KeyRight.pressed() || KeyD.pressed()) {
				plpos.x += dx * FRAME_TIME;
				//自機の右端が画面外からでないようにする
				if ((plpos.x + (txsz.x / 2)) >= 800) {
					plpos.x = 800 - txsz.x / 2;
				}
			}
		}
		//被弾時の処理
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
	//ライフを得る
	int Get_life()const {
		return life;
	}
	//リセット関数
	void reset() {
		ret = 120;
		plpos = PL_START_POS;
	}
	//ライフのリセット
	void life_reset() {
		life = 3;
	}

};
/// @brief 自機から発射される弾を管理する;
class Shot {
private:
	Size sz;
	bool bullet_max;
	Texture beam{ U"inverder_png/beam1.png" };
	const Audio sounds{U"inverder_mp3/shot.mp3"};
	double volume;
	Vec2 bpos, beamV;
	ColorF beamc;
public:
	/// @brief コンストラクタ
	/// @param[in] _sz 描画されるテクスチャのサイズ
	Shot(Size _sz) :bullet_max(false), sz(_sz), bpos(Vec2(-40, -40)), volume(0.5) {
		beamc = Palette::Seagreen;
		beamV = { 0,400 };
	}
	/// @brief 描画
	void Draw() {
		beam.resized(sz).drawAt(bpos, beamc);
	}
	void calc_shot(const Player& pl, Enemies& enemy, Walls& wall, Bonus& ufo,
		bool& anim, Vec2& apos, bool& end_flag, int& score, Stopwatch& time, bool& bonus_flag);
	/// @brief 発射物の位置を返す
	/// @return 弾の位置
	const Vec2& Get_bpos()const {
		return bpos;
	}
	/// @brief 描画サイズを返す
	/// @return 描画サイズ
	const Size& Get_sz()const {
		return sz;
	}
	/// @return テクスチャ原寸大のサイズ 
	const Size& Get_txsz()const {
		return beam.size();
	}
	void reset_shot() {
		bpos = { -40, -40 };
		bullet_max = false;
	}

};

/// @brief 被弾時アニメーション
class HitAnim {
private:
	int fpsc_e, fpsc_p;
	Size sz;
	Texture explode, anim;
	Vec2 apos;
	const Audio sound{ U"inverder_mp3/explode.mp3" };
	double volume;
	ColorF color;
public:
	/// @brief コンストラクタ
	/// @param _sz テクスチャのサイズ
	HitAnim(Size _sz) : sz(_sz), explode(U"inverder_png/delete.png"), volume(0.5),
		anim(U"inverder_png/delete_anim.png"), apos(-40, -40), color(Palette::Azure), fpsc_e(0), fpsc_p(0) {}
	/// @brief 敵被弾時アニメーション描画
	/// @param[in] pos 被弾した敵座標
	/// @param[in, out] flag アニメーションをtrueの時描画
	void DrawE(Vec2& pos, bool& flag) {
		if (flag == true) {
			if (fpsc_e >= 0 && fpsc_e <= 7) {
				sound.setVolume(volume);
				sound.play();
				explode.resized(sz).drawAt(pos, color);
			}
			else {
				sound.stop(0.5s);
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
	/// @brief プレイヤーの被弾時アニメーション描画
	/// @param[in] pos 被弾したときの自機座標
	/// @param[in, out] flag アニメーションをtrueの時描画
	void DrawP(Vec2& pos, bool& flag) {
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
	int x, y;
	/// @brief コンストラクタ
	/// @param[in] _enemy_pos 敵のテクスチャ
	/// @param[in] _anim アニメーション
	/// @param[in] _color 色
	/// @param[in] _txsz テクスチャサイズ
	/// @param[in] _epos 敵一体の座標
	/// @param[in] _score 敵一体から獲得できるスコア
	Enemy(const Texture& _enemy_pos, const Texture& _anim,ColorF _color,
		Size _txsz, Vec2 _epos, int _score, int _x, int _y) :
		Texture(_enemy_pos), anim(_anim), color(_color), txsz(_txsz), epos(_epos), score(_score)
	, x(_x), y(_y){
	}
};

class Enemies {
private:
	Array<Enemy> enemies;
	//int score;
	double dx = 20;
	double move_max;
	int move_y;
	int part = 0;
	int move_x;
	int max_enm;
	int accel;
	int cx1, cx2;
	double min_x, max_x;
	double dx2;
public:
	//コンストラクタ
	///@param[in]テクスチャのサイズ
	Enemies(const Size& txsz):move_max(100), move_y(0), min_x(0),
		max_x(0), move_x(0), accel(0), dx2(0), cx1(0), cx2(10) {
		ColorF enmyc = { 0, 0, 0 };//描画の色
		FilePathView path = U".", anim = U".";	///ファイルパスの初期化
		int point = 0;	//列ごとに設定される敵の点数
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
					txsz, Vec2(150 + x * txsz.x + x * 20, 300 - (y * txsz.y) - (y * 20)), point, x, y);
			}

		}
		max_enm = (int)enemies.max_size();
	}
	/// @brief 敵の描画
	void Draw() {
		for (const auto& enemd : enemies ) {
			//partを60で割った余りが偶数か奇数かで描画パターンを変える
			if (IsEven(part / 60)) {
				enemd.resized(enemd.txsz).drawAt(enemd.epos,
					(enemies.size() != 1) ? enemd.color : ColorF(Palette::Blueviolet));
			}
			else {
				enemd.anim.resized(enemd.txsz).drawAt(enemd.epos,
					(enemies.size() != 1) ? enemd.color : ColorF(Palette::Blueviolet));
			}
		}
		//スコアの表示
		part++;
	}
	/// @brief 敵が被弾したときの処理
	/// @param[in] bpos 自機の弾の位置
	/// @param[in] shot_sz 自機の弾のサイズ 
	/// @param[in, out] anim 被弾したとき描画するためのtrue,falseを返す, 被弾したときtrue
	/// @param[out] apos アニメーションの描画位置
	/// @param[in, out] end_flag 敵が一匹も残っていない場合はtrueを返す
	/// @param[in, out] score
	/// @return 当たったらtrueを返す
	bool hit(const Vec2& bpos, const Size& shot_sz, bool& anim, Vec2& apos, bool& end_flag, int& score) {
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
					if (enemies.empty()) {
						end_flag = true;
					}
					return true;
				}
				
			}
			it++;
		}

		return false;
	}
	/// @brief 敵の挙動
	void move() {
		int countx1 = 0, countx2 = 0;
		bool loop_f = false;
		//最大横移動距離を減算
		move_max -= abs(dx * FRAME_TIME);
		if (30 >= enemies.size() && accel == 0) {
			dx +=(dx > 0) ? 10 : -10;
			accel = 1;
		}
		if (15 >= enemies.size() && accel == 1) {
			dx += (dx > 0) ?  30 : -30;
			accel = 2;
		}
		if (enemies.size() == 1 && accel == 2) {
			dx +=(dx > 0) ? 100 : -100;
			accel = 3;
		}
		for (auto it = enemies.begin(); it != enemies.end(); ++it) {
			if (it->x == cx1 && cx1 != cx2) {
				++countx1;
			}
			if (it->x == cx2 && cx1 != cx2) {
				++countx2;
			}
		}
		if (countx1 == 0 && cx1 != cx2) {
			cx1++;
			move_max += 45;
			move_x++;
		}
		if (countx2 == 0 && cx1 != cx2) {
 			cx2--;
			move_max += 45;
			move_x++;
		}
		countx1 = 0;
		countx2 = 0;
		for(auto it = enemies.begin(); it != enemies.end(); it++){
			//横移動済みの場合縦に一キャラ分下がる
			if (move_max <= 0 && move_y != 30) {
				it->epos.y += it->txsz.y / 30;
				/*
					if (dx < 0) {
						for (auto it2 = enemies.begin(); it2 != enemies.end(); it2++, i++) {
							if (i == 0)tmp = enemies.begin()->epos.x;
							if (i >= 1 && it2->epos.x < tmp) {
								tmp = it2->epos.x;
							}
						}
						min_ix = tmp;
						tmp = 0;
						i = 0;
					}
					if (dx > 0) {

						for (auto it2 = enemies.rbegin(); it2 != enemies.rend(); it2++, i++) {
							if (i == 0) {
								tmp = enemies.rbegin()->epos.x;
							}
							if (i >= 1 && it2->epos.x > tmp) {
								tmp = it2->epos.x;
							}
						}
						max_ix = tmp;
						tmp = 0;
						i = 0;

					}
				*/
			}
			else {
				it->epos.x += dx * FRAME_TIME;
				
				if (it->epos.x < 44 && loop_f == false) {
					dx2 = 44 - it->epos.x;
					loop_f = true;
				}
				if (it->epos.x > 750 && loop_f == false) {
					dx2 = 750 - it->epos.x;
					loop_f = true;
				
				Print << dx2;
			}
		}
		}
		if (dx2 != 0) {
			for (auto it = enemies.begin(); it != enemies.end(); it++) {
				it->epos.x += dx2;
			}
			dx2 = 0.0;
			move_max = 0;
		}
		//縦移動処理
		if (move_max <= 0 && move_y != 30) {
			move_y++;
		}
		if (move_max <= 0) {
			/*
			if (dx < 0) {
				if (min_ix != 0 && min_x == 0 || min_ix < min_x) {
					min_x = min_ix;
				}
				else if (min_ix > min_x) {
					min_x = min_ix;
					move_x++;
					move_max += 45;
				}
			}
			if (dx > 0) {
				if (max_ix != 0 && max_x == 0 || max_ix > max_x) {
					max_x = max_ix;
				}
				else if (max_ix < max_x) {
					max_x = max_ix;
					move_x++;
					move_max += 45;
				}
			}
			*/
			
		}
		
		//移動方向の反転
		if (move_max <= 0 && move_y == 30) {
			dx *= -1;
			move_max = 200 + (45 * move_x);
			move_y = 0;
		}
	}

	void hitw(Walls& wall);

	
	/// @return　ランダム縦列一番下のインベーダーの座標を返す 
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

	double under_epos()const {
		double y = 0;
		for (auto it = enemies.begin(); it != enemies.end(); it++) {
			if (y == 0) {
				y = enemies.begin()->epos.y;
			}
			else {
				if (y < it->epos.y) {
					y = it->epos.y;
				}
			}
		}
		return y;
	}

	/// @brief インベーダーの配置などをリセット
	/// @param txsz テクスチャの描画サイズ
	void reset(const Size& txsz) {
		if (enemies.empty() == false) {
			enemies.clear();
		}
		ColorF enmyc = { 0, 0, 0 };
		FilePathView path = U".", anim = U".";
		int point = 0;
		part = 0;
		dx = 20, move_x = 0, accel = 0;
		min_x = 0, max_x = 0, move_max = 200;
		cx1 = 0, cx2 = 10;
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
					txsz, Vec2(150 + x * txsz.x + x * 20, 300 - (y * txsz.y) - (y * 20)), point, x, y);
			}

		}
		max_enm = (int)enemies.max_size();
	}

};

//壁一つの描画情報クラス
class Wall : public DynamicTexture{
public:
	Size sz;
	ColorF color;
	Vec2 wpos;
	Image wall;
 	/// @brief コンストラクタ
 	/// @param _wall 壁のテクスチャ
 	/// @param _sz テクスチャの描画サイズ
 	/// @param _wpos 描画する中心サイズ
 	Wall(const Image& _wall ,Size _sz, Vec2 _wpos) :DynamicTexture(_wall), sz(_sz), wpos(_wpos),
		color(Palette::Red), wall(_wall) {
		
	}
	/// @brief 壁の破壊処理
	/// @param[in] bpos 弾の位置情報
	/// @param[in] bsz 弾のサイズ
	/// @return 壁の貫通率
	double Break_wall(const Vec2& bpos, const SizeF& bsz) {
		int count = 0;	//色のある場所と重なった回数
		int x0 = (int)bpos.x;	//ループの開始位置ｘ
		int x1 = (int)bpos.x + bsz.x;	//ループ終了地点ｘ
		int y0 = (int)bpos.y;		//ループ開始位置 y
		int y1 = (int)bpos.y + bsz.y;	//ループ終了位置ｙ
		
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
				//色がある所の色を消す
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
	/// @brief コンストラクタ
	/// @param sz 描画サイズ
	Walls(const Size& sz) {
		for (int i = 0; i < 4; i++) {
			walls << Wall(Image{ U"inverder_png/zangou.png" }, sz, Vec2(150 + (i * sz.x) + (i * 65), 480));
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
					//Print << count;
					if (count >= 0.15) {
						
						return true;
					}
				}
			}
		}
		return false;
	}

	void reset(const Size& sz) {
		walls.clear();
		for (int i = 0; i < 4; i++) {
			walls << Wall(Image{ U"inverder_png/zangou.png" }, sz, Vec2(150 + (i * sz.x) + (i * 65), 480));
		}
	}
};

/// @brief ボーナスエネミーのクラス
class Bonus {
private:
	Texture Ufo{ U"inverder_png/bonus_e.png" };
	Texture break_anim{ U"inverder_png/bonus_break.png" };
	const Audio sound{ U"inverder_mp3/fly.mp3", Loop::Yes};
	double volume;
	Vec2 bpos, moveV, apos;
	ColorF color;
	Size sz;
	bool moveF;
	int anim;
public:
	/// @brief コンストラクタ
	/// @param _sz 描画サイズ
	Bonus(Size _sz) :sz(_sz), color(Palette::Crimson), moveV(Vec2{90,0}), volume(0.5)
	, bpos(Vec2(-20, 100)), apos(Vec2(-20, 100)), anim(0), moveF(false){}
	/// @brief ボーナスエネミーの描画
	/// @param hit ヒット時描画
	/// @param flag ヒット後のアニメーションが終わったか否か
	void Draw(HitAnim& hit, bool& flag) {
		if (flag == false) {
			Ufo.resized(sz).drawAt(bpos, color);
		}
		if(flag == true) {
			if (anim >= 0 && anim <= 8) {
				break_anim.resized(sz).drawAt(apos, color);
			}
			else {
				hit.DrawE(apos, flag);
				if (flag == false) {
					anim = 0;
				}
			}
			anim++;
		}
	}
	/// @brief ボーナスエネミーの動き
	/// @param time 出現までの時間
	void move(Stopwatch& time) {
		if (time >= 30s) {
			sound.play();
			if (moveF == false) {
				bpos += moveV * FRAME_TIME;
				if (bpos.x >= 800) {
					bpos = { 840, 100 };
					sound.stop();
					time.reset();
					moveF = true;
				}
			}
			else
			{
				bpos -= moveV * FRAME_TIME;
				if (bpos.x <= 0) {
					sound.stop();
					bpos = { -40, 100 };
					time.reset();
					moveF = false;
				}
			}
		}
	}
	/// @brief ボーナスエネミーのヒット判定
	/// @param[in, out] spos 自機の弾の位置 
	/// @param[in] ssz 自機弾の描画サイズ
	/// @param[in, out] time ボーナスエネミーの出現タイミング 
	/// @param[in, out] score スコアの加算
	/// @param[in, out] flag trueの時ボーナスエネミーのヒット時アニメーションを描画する
	/// @return 当たったらtrue
	bool hit(Vec2& spos, const Size& ssz, Stopwatch& time, int& score, bool& flag) {

		double sx0 = spos.x - ssz.x / 2, sx1 = spos.x + ssz.x / 2;//弾のｘ左端と、右端
		double sy0 = spos.y - ssz.y / 2 , sy1 = spos.y - ssz.y / 2;//弾のｙ上端、下端
		double x0 = bpos.x - sz.x / 2, x1 = bpos.x + sz.x / 2;//ufoのｘ左端、右端
		double y0 = bpos.y - sz.y / 2, y1 = bpos.y + sz.y / 2;//ufoｙ上端、下端
		//命中判定
		if (y0 < sy1 && sy0 < y1) {//弾のｙとufoのｙの重なり
			if (x0 < sx1 && sx0 < x1) {//弾のｘとufoのｘの重なり
				sound.stop();	//弾が命中した時点で効果音を止める
				if (moveF == false) {//動いてる方向が左
					apos = bpos;	//被弾したときのアニメーションの位置
					bpos = { -40, 100 };	//右側の出現地点
					time.reset();//出現までのカウントをリセット
					moveF = true;
					score += 300;
					flag = true;
					return true;
				}
				else {
					apos = bpos;
					bpos = { 840, 100 };
					time.reset();
					moveF = false;
					score += 300;
					flag = true;
					return true;
				}
			}
		}
		return false;
	}
	void reset() {
		bpos = { -40, 100 };
		apos = { -40, 100 };
		moveF = false;
	}
};

/// @brief 壁とエネミーが重なった時壁を削る
/// @param[in,out] wall 壁の描画情報を更新する
void Enemies::hitw(Walls& wall) {
	for (auto it = enemies.begin(); it != enemies.end(); it++) {
		if (wall.hit(it->epos, it->txsz) == true)break;
	}
}

//敵の弾の管理クラス
class Eshot {
private:
	Texture bomb{U"inverder_png/enemybomb.png"};
	Vec2 bpos, shotV;
	ColorF color;
	Size sz;	
	bool bomb_flag;
	int rate;
	double bomb_line;
	Stopwatch invincible{ StartImmediately::No };
public:
	/// @brief コンストラクタ
	/// @param _sz 描画サイズ
	Eshot(Size _sz) : bpos(Vec2(-80, -80)), shotV(Vec2(0, 200)),bomb_line(0.0),
		color(Palette::Yellow), sz(_sz), bomb_flag(false), rate(Random<int>(30, 60)){
	}
	//描画
	void Draw() {
		bomb.resized(sz).drawAt(bpos, color);
	}
	/// @brief 弾の挙動
	/// @param[in] enemy	敵の情報
	/// @param[in] pl	自機の情報
	/// @param[in, out] wall	壁の情報
	/// @param[in, out] life	残りライフ数
	/// @param[in, out] anim	trueの時アニメーションの描画を始める
	/// @param[in, out] apos	アニメーションの描画位置
	/// @param[in, out] flag	falseの時自機の移動を制限する
	void calc_eshot(const Enemies& enemy, const Player& pl, Walls& wall, Shot& shot,
		int& life, bool& anim, Vec2& apos, bool& flag, bool& hitf) {
		double sx0 = shot.Get_bpos().x - shot.Get_sz().x / 2, sx1 = shot.Get_bpos().x + shot.Get_sz().x / 2;
		double sy0 = shot.Get_bpos().y - shot.Get_sz().y / 2, sy1 = shot.Get_bpos().y + shot.Get_sz().y / 2;
		double ix0 = bpos.x - sz.x / 2, ix1 = bpos.x + sz.x / 2;
		double iy0 = bpos.y - sz.y / 2, iy1 = bpos.y + sz.y / 2;
		if (hitf == true) {
			invincible.start();
		}
		if (invincible > 1.5s) {
			hitf = false;
			invincible.reset();
		}

		if (bomb_flag == false && rate == 0) {	//弾が画面上にないかつ発射レートが0のとき
			bpos = enemy.rand_epos();	//弾の発射位置
			bomb_line = bpos.y;		//弾のｙ位置を保存
			bomb_flag = true;		//弾の発射フラグをtrue
		}
		
		else {//弾射出中			
			
			if (bomb_line + 30 >= 520) {
				bomb_flag = false;
				bpos = { -40,-40 };
				rate = Random<int>(30, 60); //30～60のランダムレート（フレーム）
			}
			else {
				
				bpos += shotV * FRAME_TIME;
				if (bpos.y >= 600) {//弾が画面下に到達したら消す
					bomb_flag = false;
					bpos = { -40, -40 };
					rate = Random<int>(30, 60);
				}
				//自機に命中した時
				if (hitf == false && (pl.jikipos().y - pl.jikisz().y / 2 <= bpos.y + sz.y / 2)) {
					if ((pl.jikipos().x - pl.jikisz().x <= bpos.x - sz.x &&
						pl.jikipos().x + pl.jikisz().x >= bpos.x - sz.x) ||
						(pl.jikipos().x - pl.jikisz().x <= bpos.x + sz.x &&
						pl.jikipos().x + pl.jikisz().x >= bpos.x + sz.x)) {
						life--;
						anim = true;
						flag = false;
						apos = pl.jikipos();
						hitf = true;
						bomb_flag = false;
						bpos = { -40, -40 };
						rate = Random<int>(30, 60);
					}
				}
				//壁に命中した時
				if (wall.hit(bpos, sz) == true) {
					bomb_flag = false;
					//Print << U"!!!";
					bpos = { -40, -40 };
					rate = Random<int>(30, 60);
				}
				if (iy0 < sy1 && sy0 < iy1) {
					if (ix0 < sx1 && sx0 < ix1) {
						anim = true;
						bomb_flag = false;
						bpos = { -40, -40 };
						rate = Random<int>(30, 60);
						shot.reset_shot();
					}
				}
			}
			rate--;
		}
	}
	//現在描画されている弾を消す
	void Delete_bomb() {
		bomb_flag = false;
		bpos = { -40, -40 };
		rate = Random<int>(30, 60);
	}
};


/// @brief 自機の弾の動き
/// @param[in] pl 自機の情報
/// @param[in] enemy 敵の情報
/// @param[in] wall 壁の情報
/// @param[in]ufo ボーナスエネミーの情報
/// @param[in, out] anim trueの時アニメーションの描画
/// @param[in, out] apos アニメーションの描画位置
/// @param[in, out] end_flag ゲーム終了判定、trueの時終了
/// @param[in, out] score 獲得したスコア
/// @param[in, out] time ボーナスエネミーの出現時間
/// @param[in, out] bonus_flag ボーナスエネミーの出現フラグ
void Shot::calc_shot(const Player& pl, Enemies& enemy, Walls& wall, Bonus& ufo,
	bool& anim, Vec2& apos, bool& end_flag, int& score, Stopwatch& time, bool& bonus_flag) {
	if (bullet_max == false ) {		//画面上に自機の弾がない
		if (KeySpace.pressed()) {
			bpos = pl.jikipos();
			bullet_max = true;
			sounds.setVolume(volume);
			sounds.playOneShot();

		}
	}
	//画面上に弾がある
	if (bullet_max == true) {
		bpos -= beamV * FRAME_TIME;
		//弾が画面外に行った時弾を消す
		if (bpos.y < 0) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
		if (enemy.hit(bpos, sz, anim, apos, end_flag, score) == true) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
		if (wall.hit(bpos, sz) == true) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
		if (ufo.hit(bpos, sz, time, score, bonus_flag) == true) {
			bpos = { -40, -40 };
			bullet_max = false;
		}
	}
	
}

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ Palette::Black });
	Start start;
	Over Over;
	String Game_State = U"start";
	Size txsz_chara{ 30, 30 };
	Size txsz_bullet{ 15, 20 };
	Size txsz_obj{ 100,100 };
	Font font{20};
	bool end_flag = false, ufo_flag = false;
	Rect debug_rect{ Arg::center(400, 300), 30, 30 };
	int life = 3, score = 0;
	Player pl(life, PL_START_POS, txsz_chara);
	Enemies enemy(txsz_chara);
	Walls wall(txsz_obj);
	Shot shot(txsz_bullet);
	Eshot bomb(txsz_bullet);
	HitAnim anim(txsz_chara);
	Bonus ufo(txsz_chara);
	Stopwatch time{ StartImmediately::No }, end_time{StartImmediately::No};
	bool exflag_e = false, exflag_p = false, move_flag = true;
	Vec2 apos_e{ -40,-40 }, apos_p{ -40, -40 };
	Font font_life{ 20 };
	bool pl_invincible = true;

	while (System::Update())
	{
		if (Game_State == U"start") {
			start.View();
			if (KeyF.pressed()) {
				Game_State = U"game";
			}
		}
		if (Game_State == U"game") {
			if (end_flag == false) {
				if (time <= 0s) {
					time.start();
				}
				//Print << time;
				if (life <= 0 || enemy.under_epos() + txsz_chara.y > 550) {
					Game_State = U"over";
				}
				pl.move(move_flag);
				enemy.move();
				ufo.move(time);
				shot.calc_shot(pl, enemy, wall, ufo, exflag_e, apos_e, end_flag, score, time, ufo_flag);
				shot.Draw();
				enemy.hitw(wall);
				bomb.calc_eshot(enemy, pl, wall, shot, life, exflag_p, apos_p, move_flag, pl_invincible);
				bomb.Draw();
				anim.DrawE(apos_e, exflag_e);
				anim.DrawP(apos_p, exflag_p);
				ufo.Draw(anim, ufo_flag);
				wall.Draw();
				pl.Draw();
				enemy.Draw();
				font_life(U"Life", life).draw(Arg::bottomLeft(0, 600));
				font(U"Score\n", score).draw(0, 0);
			}
			if (end_flag == true) {
				end_time.start();
				if (end_time <= 1s) {
					bomb.Delete_bomb();
					pl.Draw();
					wall.Draw();
					anim.DrawE(apos_e, exflag_e);
					font_life(U"Life", life).draw(Arg::bottomLeft(0, 600));
					font(U"Score\n", score).draw(0, 0);
				}
				else {
					wall.reset(txsz_obj);
					pl.reset();
					enemy.reset(txsz_chara);
					ufo.reset();
					time.reset();
					end_time.reset();
					end_flag = false;
					System::Sleep(1s);
				}
			}
		}
		if (Game_State == U"over") {
			Over.View();
			if (KeyR.pressed()) {
				wall.reset(txsz_obj);
				pl.reset();
				life = 3;
				score = 0;
				enemy.reset(txsz_chara);
				ufo.reset();
				end_flag = false;
				System::Sleep(1s);
			}
			if (KeyQ.pressed()) {
				System::Exit();
			}
		}
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
