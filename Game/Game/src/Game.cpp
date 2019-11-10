
#include "Game.hpp"
#define font FontAsset(U"Display")
//FontAsset(U"Score")(m_score).drawAt(Scene::Center().x, 30);
typedef struct {
	int x;
	int y;
	int status;
	int turn;
}Coordinate;

Coordinate record[64] = { 0 }; //��育�ƂɋL�^����
int innerCell[10][10] = { 0 }; //�΂̏ꏊ���L������ �̈�O���܂�
int deepCell[100][10][10] = { 0 }; //������ǂޗp
int turn = 1; //�萔�𐔂���

int stack[60] = { 0 };
int sp = 0;

bool pleaseSkip = false;

int flip_stone(int, Coordinate, int);
void push(int);
int pop();
int staticScore(int);
int cellScore(int, int, int);



//Window::SetTitle(U"Reversi");
const Coordinate Origin = { 170, 90 };

const int32 gridSize = 60;
const Rect rectFrame(Origin.x, Origin.y, gridSize * 8, gridSize * 8);
const Line LineX(Origin.x + 60, Origin.y, Origin.x + 60, Origin.y + 480);
const Line LineY(Origin.x, Origin.y + gridSize, Origin.x + 480, Origin.y + gridSize);
Rect cell[8][8]; //�N���b�N���ꂽ�̈�����o���邾��
Circle stone[8][8]; //�΂̌`��A�傫�������߁A���W������Đ΂�\������
Circle smallStone[8][8];
//const Font font(100); //���b�Z�[�W��\�����邽�߂ɗp��

bool search = true;  //�T�����邩���Ȃ������߂�
bool ai = true;      //�R���s���[�^�ΐ킷�邩���Ȃ���
bool put = false;    //�΂��u���ꂽ������true�ɂ���


int32 canPut[10][10] = { 0 };

int32 oncePerSecond = 0; //1�b�Ɉ�񂭂炢���s���������̂Ɏg��

Game::Game(const InitData& init)
	: IScene(init)
{
	// �� (Scene::Width() / blockSize.x) �A�c 5 �̃u���b�N��z��ɒǉ�����
	for (auto p : step(Size((Scene::Width() / blockSize.x), 5)))
	{
		m_blocks << Rect(p.x * blockSize.x, 60 + p.y * blockSize.y, blockSize);
	}

	//���S���W��������
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cell[i][j] = Rect(Origin.x + gridSize * i, Origin.y + gridSize * j, 60);
			stone[i][j] = Circle(Origin.x + gridSize * i + gridSize / 2, Origin.y + gridSize * j + gridSize / 2, 25);
			smallStone[i][j] = Circle(Origin.x + gridSize * i + gridSize / 2, Origin.y + gridSize * j + gridSize / 2, 5);
		}
	}

	//�ǂ̕����̔z���-1������
	for (int i = 0; i < 10; i++)
	{
		innerCell[i][0] = -1;
		innerCell[i][9] = -1;
		innerCell[0][i] = -1;
		innerCell[9][i] = -1;
		for (int j = 0; j < DEPTH; j++) {
			deepCell[j][i][0] = -1;
			deepCell[j][i][9] = -1;
			deepCell[j][0][i] = -1;
			deepCell[j][9][i] = -1;
		}
	}
	innerCell[4][4] = deepCell[1][4][4] = 1;
	innerCell[4][5] = deepCell[1][4][5] = 2;
	innerCell[5][4] = deepCell[1][5][4] = 2;
	innerCell[5][5] = deepCell[1][5][5] = 1;
	Window::SetTitle(U"Reversi");

	
  int32 tmpTime = 0;
}

void Game::update()
{
	/****************************************************************************/
	/*
	// �p�h���𑀍�
	m_paddle = Rect(Arg::center(Cursor::Pos().x, 500), 60, 10);

	// �{�[�����ړ�
	m_ball.moveBy(m_ballVelocity * Scene::DeltaTime());

	// �u���b�N�����Ƀ`�F�b�N
	for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		// �{�[���ƃu���b�N���������Ă�����
		if (it->intersects(m_ball))
		{
			// �{�[���̌����𔽓]����
			(it->bottom().intersects(m_ball) || it->top().intersects(m_ball) ? m_ballVelocity.y : m_ballVelocity.x) *= -1;

			// �u���b�N��z�񂩂�폜�i�C�e���[�^�������ɂȂ�̂Œ��Ӂj
			m_blocks.erase(it);

			// �X�R�A�����Z
			++m_score;

			// ����ȏ�`�F�b�N���Ȃ�  
			break;
		}
	}

	// �V��ɂԂ�������͂˕Ԃ�
	if (m_ball.y < 0 && m_ballVelocity.y < 0)
	{
		m_ballVelocity.y *= -1;
	}

	if (m_ball.y > Scene::Height())
	{
		changeScene(State::Title);
		getData().highScore = Max(getData().highScore, m_score);
	}

	// ���E�̕ǂɂԂ�������͂˕Ԃ�
	if ((m_ball.x < 0 && m_ballVelocity.x < 0) || (Scene::Width() < m_ball.x && m_ballVelocity.x > 0))
	{
		m_ballVelocity.x *= -1;
	}

	// �p�h���ɂ���������͂˕Ԃ�
	if (m_ballVelocity.y > 0 && m_paddle.intersects(m_ball))
	{
		// �p�h���̒��S����̋����ɉ����Ă͂˕Ԃ������ς���
		m_ballVelocity = Vec2((m_ball.x - m_paddle.center().x) * 10, -m_ballVelocity.y).setLength(speed);
	}

	*/
	/****************************************************************************/

	oncePerSecond++;
  
	if (getData().tmpTime != (int32)Scene::Time()) {
		oncePerSecond = 0;
    getData().tmpTime = (int32)Scene::Time();
	}
	//�Ֆ�
	Scene::SetBackground(Palette::Green);
	rectFrame.drawFrame(0, 3, Palette::Black);
	//LineX.draw(3, Palette::Black);
	//LineY.draw(3, Palette::Black);
	for (int i = 1; i < 8; i++) {
		Line(Origin.x + gridSize * i, Origin.y, Origin.x + gridSize * i, Origin.y + gridSize * 8).draw(3, Palette::Black);
		Line(Origin.x, Origin.y + gridSize * i, Origin.x + gridSize * 8, Origin.y + gridSize * i).draw(3, Palette::Black);
	}

  //cell���ƂɃX�R�A�v�Z
  int32 score = 0;
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      score = cellScore(i, j, 1);
      FontAsset(U"cellNum")(score).draw(Origin.x + gridSize * (i - 1) + 8, Origin.y + gridSize * (j - 1) + 8, Palette::Black);
      if (score >= 0) {
      }
    }
  }
	//�΂�u����ꏊ��������
	Coordinate c;

	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			canPut[i][j] = 0;
		}
	} 
  
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			c = { i + 1, j + 1, (turn + 2) % 2 + 1, turn }; //�T���֐��ɑ�����
			//if (oncePerSecond % 3 == 0)
				for (int k = 0; k < 8; k++) { //�T�����������߂�

					canPut[i + 1][j + 1] += flip_stone(k, c, 0); //�Ђ�����Ԃ��Ȃ�
					if (innerCell[i + 1][j + 1] != 0) {
						canPut[i + 1][j + 1] = 0; //�u����ꏊ�łȂ�������0�ɖ߂�
					}
				}
			if (canPut[i + 1][j + 1] > 0) { //�Ђ�����Ԃ蓾�鐔��\��
				font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i, Origin.y + gridSize * j, Palette::White);

			}
			else {
				//font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i, Origin.y + gridSize * j, Palette::Black);
			}
		}
	}
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      canPut[i][j] = 0;
    }
  }
  
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      ///////////���̕���
      c = { i + 1, j + 1, (turn + 1) % 2 + 1, turn }; //�T���֐��ɑ�����
      //if (oncePerSecond % 3 == 0)
      for (int k = 0; k < 8; k++) { //�T�����������߂�

        canPut[i + 1][j + 1] += flip_stone(k, c, 0); //�Ђ�����Ԃ��Ȃ�
        if (innerCell[i + 1][j + 1] != 0) {
          canPut[i + 1][j + 1] = 0; //�u����ꏊ�łȂ�������0�ɖ߂�
        }
      }
      if (canPut[i + 1][j + 1] > 0) { //�Ђ�����Ԃ蓾�鐔��\��
        font(canPut[i + 1][j + 1]).draw(Origin.x + gridSize * i +5, Origin.y + gridSize * j + 5, Palette::White);
        smallStone[i][j].draw(Palette::Darkgoldenrod);
      }
      //////////�I���
    }
  }
  
  

	//�N���b�N�����Ƃ���ɐ΂�u��
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (cell[i][j].leftClicked() && innerCell[i + 1][j + 1] == 0 && canPut[i + 1][j + 1] > 0) {
				turn++;
				innerCell[i + 1][j + 1] = turn % 2 + 1; //1�Ȃ獕�A2�Ȃ甒
				record[turn].x = i + 1; //�u�����ꏊ���L�^
				record[turn].y = j + 1;
				record[turn].turn = turn;
				record[turn].status = innerCell[i + 1][j + 1]; //�F���L�^
				put = true; //�T��������
			}

			if (innerCell[i + 1][j + 1] == 1) {
				//stone[i][j].draw(Palette::Black);
			}
			else if (innerCell[i + 1][j + 1] == 2) {
				//stone[i][j].draw(Palette::White);
			}
      //�΂�\��
			if (deepCell[turn][i + 1][j + 1] == 1) {
				stone[i][j].draw(Palette::Black);
			}
			else if (deepCell[turn][i + 1][j + 1] == 2) {
				stone[i][j].draw(Palette::White);
			}
		}
	}


	//�����_���ɒu��
	int maxPut = 0;
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			maxPut = Max(canPut[i][j], maxPut);
		}
	}
	//Print << U"max:" << maxPut;
	int32 I, J;
  SimpleGUI::CheckBox(ai, U"Computer", Vec2(10, 150));
	if (ai == true && turn % 2 == 0) {
		int32 m = Random() * 100, n = Random() * 100;
		for (int i = 1; i < 9; i++) {
			for (int j = 1; j < 9; j++) {
				if (m >= 50 && n >= 50) {
					I = i;
          J = j;
				}
				else if(m >= 50 && n < 50){
					I = j;
          J = i;
				} 
        else if (m < 50 && n >= 50) {
          I = j;
					J = 9 - i;
				}
				else if(true){
          I = 9 - j;
					J = 9 - i;
        }
        else if (false) {

        }
      
				if (innerCell[I][J] == 0 && canPut[I][J] > 0 && (canPut[I][J] == maxPut /*|| Random()*100 > turn + 30*/)) {
					turn++;
					innerCell[I][J] = turn % 2 + 1; //1�Ȃ獕�A2�Ȃ甒
					record[turn].x = I; //�u�����ꏊ���L�^
					record[turn].y = J;
					record[turn].turn = turn;
					record[turn].status = innerCell[I][J]; //�F���L�^
					put = true; //�T��������
				}
				if (put == true) {
					break;
				}
			}
			if (put == true) {
				break;
			}
		}
		//�X�L�b�v���Ă��炤
		if (put == false) {
			//turn++;
      pleaseSkip = true;
			font(U"�X�L�b�v���Ă�������").draw(Origin.x + gridSize * 2, Origin.y + gridSize * 3);
		}
	}

  if (pleaseSkip == true) {
    font(U"�X�L�b�v���Ă�������").draw(Origin.x + gridSize * 2, Origin.y + gridSize * 3);
  }
	//���̃^�[���Œu���ꂽ�ꏊ�Ɉ������
	if (record[turn].x > 0 && record[turn].y > 0) {
		smallStone[record[turn].x - 1][record[turn].y - 1].draw(Palette::Aqua);
	}

	//�ύX��deepCell�ɂ����f������
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			deepCell[turn][i][j] = innerCell[i][j];
		}
	}

	int ans = 0;
	//�΂������łЂ�����Ԃ�
	//SimpleGUI::CheckBox(search, U"�΂�Ԃ�", Vec2(500, 40));
	if (search == true && put == true) { //�`�F�b�N�{�b�N�Xtrue�Ő΂��u���ꂽ��T��
		ans = 0;
		for (int k = 0; k < 8; k++) {
			ans += flip_stone(k, record[turn], -1); //�Ђ�����Ԃ�
		}
	}
	//Print << U"�Ԃ����΂̐���"<<ans;

	//�萔�A���Ԃ�\��
	font(U"�萔", turn).draw(10, 200);
	if (turn % 2 == 0) {
		font(U"���̔�").draw(10, 250);
	}
	else {
		font(U"���̔�").draw(10, 250);
	}

	//�΂̐���\��
	int32 black = 0, white = 0;
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			if (innerCell[i][j] == 1) {
				black++;
			}
			else if (innerCell[i][j] == 2) {
				white++;
			}
		}
	}
	font(black).draw(700, 100, Palette::Black);
	font(white).draw(700, 150);

	if (oncePerSecond % 60 == 0) {
		Print << staticScore(2);
	}
	put = false; //���ɖ߂�


	//Print << record[turn].x <<record[turn].y<<record[turn].status;

	//�΂���������{�^��
	if (SimpleGUI::Button(U"Clear", Vec2(10, 50))) {
		for (int i = 1; i < 9; i++) {
			for (int j = 1; j < 9; j++) {
				innerCell[i][j] = 0;
				record[i * 10 + j] = { 0 }; //�L�^�����Z�b�g
				for (int t = 0; t < DEPTH; t++) {
					deepCell[t][i][j] = 0;
				}
			}
		}
		turn = 1; //�萔�����Z�b�g
		innerCell[4][4] = deepCell[1][4][4] = 1;
		innerCell[4][5] = deepCell[1][4][5] = 2;
		innerCell[5][4] = deepCell[1][5][4] = 2;
		innerCell[5][5] = deepCell[1][5][5] = 1;
    pleaseSkip = false;
	}

	//�X�L�b�v����{�^��
	if (SimpleGUI::Button(U"Skip", Vec2(10, 100))) {
		turn++;
    pleaseSkip = false;
	}

  //�^�C�g����ʂɖ߂�
	if (SimpleGUI::Button(U"end", Vec2(10,500))) {
    changeScene(State::Title);
    getData().highScore = 999;
      //Max(getData().highScore, m_score);
	}
  
}

void Game::draw() const
{
	/****************************************************************************/
	/*
	FontAsset(U"Score")(m_score).drawAt(Scene::Center().x, 30);

	// ���ׂẴu���b�N��`�悷��
	for (const auto& block : m_blocks)
	{
		block.stretched(-1).draw(HSV(block.y - 40));
	}

	// �{�[����`��
	m_ball.draw();

	// �p�h����`��
	m_paddle.draw();
	*/
	/****************************************************************************/

}

int flip_stone(int k, Coordinate c, int a) {
	int flag = 0;
	int X = 0, Y = 0, Z = 0;
  if (a >= 1) {
		Z = a;
	}

	//switch���ŒT�����������߂�
	switch (k) {
	case 0:
		X = 1, Y = 0;
		break;
	case 1:
		X = 1; Y = 1;
		break;
	case 2:
		X = 0, Y = 1;
		break;
	case 3:
		X = -1; Y = 1;
		break;
	case 4:
		X = -1, Y = 0;
		break;
	case 5:
		X = -1, Y = -1;
		break;
	case 6:
		X = 0, Y = -1;
		break;
	case 7:
		X = 1, Y = -1;
		break;
	default:
		break;
	}

	//�T����
	int i = c.x + X, j = c.y + Y;
	int n = 0;
	int m = 0;
	int ans = 0;
	int tmp = 0;

	sp = 0;
	while (1)
	{
		//Print << c.turn;
		if (deepCell[c.turn][i][j] > 0) {
			for (int count = 0; deepCell[c.turn][i][j] != c.status && deepCell[c.turn][i][j] > 0; count++) {
				push(i * 10 + j);
				n++;
				//Print << U"push���ꂽ"<<i*10 + j;
				i += X;
				j += Y;
				m = 1;
			}
			//break;
			if (deepCell[c.turn][i][j] == c.status && m == 1) {
				while (n > 0) {
					tmp = pop();
					if (a == -1) {
						innerCell[tmp / 10][tmp % 10] = c.status;
						deepCell[c.turn][tmp / 10][tmp % 10] = c.status;
					}
					else if (a >= 1) {
						deepCell[c.turn][tmp / 10][tmp % 10] = c.status;
					}
					//Print << U"pop���ꂽ"<<tmp;
					ans++;
					n--;
				}
			}
		}
		break;
	}
	return ans;
}

void push(int i) {
	stack[sp++] = i;
}

int pop() {
	int tmp = stack[--sp];
	stack[sp + 1] = 0;
	return tmp;
}

//d���ł̃X�R�A���v�Z����
int staticScore(int d) {
  d += turn;
	//s = 0;
	//int k = 0;
	int ans = 0;
	//int canPut[10][10] = { 0 };
  int s = 0;
	while (s < 2)
	{
		s++;
	}
	

  //���̔Ԃ̃v���C���[���u����ꏊ�̐����J�E���g
	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			for (int k = 0; k < 8; k++) {
				Coordinate c = { i, j, (d + 1) % 2 + 1, d + 1 };
				canPut[i][j] += flip_stone(k, c, 0); //�Ђ�����Ԃ��Ȃ�
				if (innerCell[i][j] != 0) {
					canPut[i][j] = 0; //�u����ꏊ�łȂ�������0�ɖ߂�
				}
			}
		}
	}
	for (int i = 1; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (canPut[i][j] > 0) {
				ans++;
			}
		}
	}
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      canPut[i][j] = 0;
    }
  }
  
  //����v���C���|���u����ꏊ�̐����J�E���g
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      for (int k = 0; k < 8; k++) {
        Coordinate c = { i, j, (d + 2) % 2 + 1, d + 1 };
        if (d > turn) {
          canPut[i][j] += flip_stone(k, c, 0); //�Ђ�����Ԃ��Ȃ�
        }
        else {
          canPut[i][j] += flip_stone(k, c, 0); //�Ђ�����Ԃ��Ȃ�
        }
        if (innerCell[i][j] != 0) {
          canPut[i][j] = 0; //�u����ꏊ�łȂ�������0�ɖ߂�
        }
      }
    }
  }
  for (int i = 1; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (canPut[i][j] > 0) {
        ans--;
      }
    }
  }
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      canPut[i][j] = 0;
    }
  }
  
  ans *= 10;
  int tu = (turn - 1) % 2 + 1;
  
  //�p
  if (deepCell[d][1][1] == tu) {
    ans += 5;
    if (deepCell[d][1][2] == tu || deepCell[d][2][1] == tu || deepCell[d][2][2] == tu) {
      ans += 10;
    }
    //Print << U"�p";
  }
  if (deepCell[d][1][8] == tu) {
    ans += 5;
    if (deepCell[d][1][7] == tu || deepCell[d][2][8] == tu || deepCell[d][2][7] == tu) {
      ans += 10;
    }
    //Print << U"�p";
  }
  if (deepCell[d][8][1] == tu) {
    ans += 5;
    if (deepCell[d][8][2] == tu || deepCell[d][7][1] == tu || deepCell[d][7][2] == tu) {
      ans += 10;
    }
    //Print << U"�p";
  }
  if (deepCell[d][8][8] == tu) {
    ans += 5;
    if (deepCell[d][8][7] == tu || deepCell[d][7][8] == tu || deepCell[d][7][7] == tu) {
      ans += 10;
    }
    //Print << U"�p";
  }

  //1,1�̎���
  //��
  if (deepCell[d][3][1] == tu) {
    if (deepCell[d][2][1] == tu && deepCell[d][1][1] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][2][1] == 0 && deepCell[d][1][1] == 0) {
      ans += 1;
    }
  }
  //�΂�
  if (deepCell[d][3][3] == tu) {
    if (deepCell[d][2][2] == tu && deepCell[d][1][1] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][2][2] == 0 && deepCell[d][1][1] == 0) {
      ans += 1;
    }
  }
  //�c
  if (deepCell[d][1][3] == tu) {
    if (deepCell[d][1][2] == tu && deepCell[d][1][1] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][1][2] == 0 && deepCell[d][1][1] == 0) {
      ans += 1;
    }
  }
  //8,1�̎���
  //��
  if (deepCell[d][6][1] == tu) {
    if (deepCell[d][7][1] == tu && deepCell[d][8][1] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][7][1] == 0 && deepCell[d][8][1] == 0) {
      ans += 1;
    }
  }
  //�΂�
  if (deepCell[d][6][3] == tu) {
    if (deepCell[d][7][2] == tu && deepCell[d][8][1] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][7][2] == 0 && deepCell[d][8][1] == 0) {
      ans += 1;
    }
  }
  //�c
  if (deepCell[d][8][3] == tu) {
    if (deepCell[d][8][2] == tu && deepCell[d][8][1] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][8][2] == 0 && deepCell[d][8][1] == 0) {
      ans += 1;
    }
  }
  //1,8�̎���
  //��
  if (deepCell[d][3][8] == tu) {
    if (deepCell[d][2][8] == tu && deepCell[d][1][8] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][2][8] == 0 && deepCell[d][1][8] == 0) {
      ans += 1;
    }
  }
  //�΂�
  if (deepCell[d][3][6] == tu) {
    if (deepCell[d][2][7] == tu && deepCell[d][1][8] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][2][7] == 0 && deepCell[d][1][8] == 0) {
      ans += 1;
    }
  }
  //�c
  if (deepCell[d][1][6] == tu) {
    if (deepCell[d][1][7] == tu && deepCell[d][1][8] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][1][7] == 0 && deepCell[d][1][8] == 0) {
      ans += 1;
    }
  }
  //8,8�̎���
  //��
  if (deepCell[d][6][8] == tu) {
    if (deepCell[d][7][8] == tu && deepCell[d][8][8] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][7][8] == 0 && deepCell[d][8][8] == 0) {
      ans += 1;
    }
  }
  //�΂�
  if (deepCell[d][6][6] == tu) {
    if (deepCell[d][7][7] == tu && deepCell[d][8][8] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][7][7] == 0 && deepCell[d][8][8] == 0) {
      ans += 1;
    }
  }
  //�c
  if (deepCell[d][8][6] == tu) {
    if (deepCell[d][8][7] == tu && deepCell[d][8][8] == 0) {
      ans -= 5;
    }
    else if (deepCell[d][8][7] == 0 && deepCell[d][8][8] == 0) {
      ans += 1;
    }
  }
  
	return ans;
}

//x��1-8,y��1-8, z�͐[��
//cell���Ƃ̃X�R�A��t����
//�u���Ȃ��Ƃ���Ȃ�0
int cellScore(int x, int y, int z) { 
  //int ans = deepCell[z + turn][x][y];
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      for (int k = 1; k < z + 1; k++) {
        deepCell[turn + z][i][j] = deepCell[turn][i][j];
      }
    }
  }

  Coordinate c ;
  int32 score = 0;
  //���肵�Ă݂ăX�R�A��t����
  if (deepCell[z + turn][x][y] == 0) {
    
    deepCell[z + turn][x][y] = (turn + 1) % 2 + 1; //1�Ȃ獕�A2�Ȃ甒
    c.x = x; //�u�����ꏊ���L�^
    c.y = y;
    c.turn = turn + z;
    c.status = deepCell[z + turn][x][y]; //�F���L�^
    //put = true; //�T��������

    for (int k = 0; k < 8; k++) {
      int ans = flip_stone(k, c, z); //�Ђ�����Ԃ�
    }
    score = staticScore(z);
  }
  
  //�X�R�A�v�Z��������Ƃɖ߂�
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j < 9; j++) {
      for (int k = 1; k < z + 1; k++) {
        deepCell[k + turn][i][j] = deepCell[turn][i][j];
      }
    }
  }
  
  return score;
}
