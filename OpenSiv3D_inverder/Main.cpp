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

class Enemies;

const Vec2 PL_START_POS{ 400, 550 };


class Player {
private:
	int life;
	Texture jiki{ U"inverder_png/jiki.png" };
	Vec2 plpos;
	Size txsz;
	double dx;
public:
	Player(int _life, Vec2 _plpos, Size _txsz) :life(_life), plpos(_plpos), dx(300), txsz(_txsz){
		
	}
	void move() {
		if (KeyLeft.pressed()|| KeyA.pressed()) {
			plpos.x -= dx * Scene::DeltaTime();
			if ((plpos.x - (txsz.x / 2)) <= 0) {
				plpos.x = 0 + txsz.x / 2;
			}
		}
		if (KeyRight.pressed() || KeyD.pressed()) {
			plpos.x += dx * Scene::DeltaTime();
			if ((plpos.x + (txsz.x / 2)) >= 800) {
				plpos.x = 800 - txsz.x / 2;
			}
		}
	}
	void Draw() {
		jiki.resized(txsz).drawAt(plpos);
	}
	const Vec2& jikipos()const {
		return plpos;
	}
	
};
class Shot {
private:
	Size sz;
	bool bullet_max;
	Texture beam{ U"inverder_png/beam1.png" };
	Vec2 bpos, beamV;
	ColorF beamc;
public:
	Shot(Size _sz, Vec2 _bpos):bullet_max(false), sz(_sz), bpos(_bpos) {
		beam(bpos, sz);
		beamc = Palette::Seagreen;
		beamV = { 200,200 };
	}
	void Draw() {
		beam.draw(beamc);
	}
	void calc_shot(const Player& pl, Enemies& enemy); 
	const Vec2& Get_bpos()const {
		return bpos;
	}
	const Size& Get_sz()const {
		return sz;
	}
};

class Enemy : public Texture{
public:
	Size txsz;
	ColorF color;
	Vec2 epos;
	Texture anim;
	Enemy(const Texture& _enemy_pos, const Texture& _anim,ColorF _color, Size _txsz, Vec2 _epos) :
		Texture(_enemy_pos), anim(_anim), color(_color), txsz(_txsz), epos(_epos){		
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
public:
	//コンストラクタ
	///@param[in]テクスチャのサイズ
	Enemies(const Size& txsz):move_max(100), move_y(0) {
		ColorF enmyc = { 0, 0, 0 };
		FilePathView path = U".", anim = U".";
		score = 0;
		for (int y = 0; y < 5; y++) {
			for (int x = 0; x < 11; x++) {
				switch (y)
				{
				
				case 0:
					enmyc = { Palette::Magenta };
					path = U"inverder_png/enemy1.png";
					anim = U"inverder_png/enemy1_anim.png";
					break;
				case 1:
					enmyc = { Palette::Magenta };
					path = U"inverder_png/enemy1.png";
					anim = U"inverder_png/enemy1_anim.png";
					break;
				case 2:
					enmyc = { Palette::Aqua };
					path = U"inverder_png/enemy2.png";
					anim = U"inverder_png/enemy2_anim.png";
					break;
				case 3:
					enmyc = { Palette::Aqua };
					path = U"inverder_png/enemy2.png";
					anim = U"inverder_png/enemy2_anim.png";
					break;
				case 4:
					enmyc = { Palette::Greenyellow };
					path = U"inverder_png/enemy3.png";
					anim = U"inverder_png/enemy3_anim.png";
					break;
				default:
					break;
				}
				enemies << Enemy(Texture{ path }, Texture{ anim }, enmyc,
					txsz, Vec2(150 + x * txsz.x + x * 20, 300 - (y * txsz.y) - (y * 20)));
			}

		}
	}

	void Draw() {
		for (const auto& enemd : enemies ) {
			if (IsEven(part / 60)) {
				enemd.resized(enemd.txsz).drawAt(enemd.epos, enemd.color);
			}
			else {
				enemd.anim.resized(enemd.txsz).drawAt(enemd.epos, enemd.color);
			}
		}
		part++;
	}
	bool hit(const Shot& shot) {
		for (auto it = enemies.begin(); it != enemies.end();) {
			if ((it->epos.y + it->txsz.y / 2) >= shot.Get_bpos().y + shot.Get_sz().y/2
				&& it->epos.x + it->txsz.x/2 <= shot.Get_bpos().x || it->epos.x - it->txsz.x/2 >= shot.Get_bpos().x) {
				it = enemies.erase(it);
				score += 20;
				return true;
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
};

class Wall : public Texture{
public:
	Size sz;
	Texture wall;
	ColorF color;
	Vec2 wpos;
	Wall(const Texture& _wall ,Size _sz, Vec2 _wpos) :wall(_wall), sz(_sz), wpos(_wpos) {
		color = Palette::Red;
	}
};

class Walls {
private:
	Array<Wall> walls;
public:
	Walls(Size sz) {
		for (int i = 0; i < 4; i++) {
			walls << Wall(Texture{ U"inverder_png/zangou.png" }, sz, Vec2(150 + (i * sz.x), 500));
		}
	}
	void Draw() {
		for (const auto& walld : walls) {
			walld.resized(walld.sz).drawAt(walld.wpos, walld.color);
		}
	}
};

void Shot::calc_shot(const Player& pl, Enemies& enemy) {
	if (bullet_max == false) {
		if (KeySpace.pressed()) {
			bpos = pl.jikipos();
			bullet_max = true;
		}
	}
	if (bullet_max == true) {
		bpos += beamV * Scene::DeltaTime();
		if (bpos.y > 600) {
			bullet_max = false;
		}
	}
	
}

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ Palette::Black });

	Size txsz_chara{ 30, 30 };
	Size txsz_bullet{ 10, 10 };
	Size txsz_obj{ 100,100 };

	Rect debug_rect{ Arg::center(400, 300), 30, 30 };

	Player pl(3, PL_START_POS, txsz_chara);
	Enemies enemy(txsz_chara);
	Walls wall(txsz_obj);


	while (System::Update())
	{
		wall.Draw();
		pl.Draw();
		enemy.Draw();
		pl.move();
		enemy.move();
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
