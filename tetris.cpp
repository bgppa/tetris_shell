#include <iostream>
#include <ncurses.h>
#include <vector>

using namespace std;

string tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;

int nScreenWidth = 0; //80;
int nScreenHeight = 0; //30;

/* For a 2D Matrix, with x,y coordinate, we can transform them into 1D
 * coordinates as done usually in scientific computing (e.g. Lapack).
 * We apply this principle here: given the x-y coordinates in 2D for a 
 * tetromino, return the corresponding 1D vector coordinate.
 * Set r = 1, 2, 3, 0 for the coord. after a rotation */
int Rotate (int px, int py, int r)
{
  switch (r % 4)
  {
    case 0: return py * 4 + px;         // 0 degrees...
    case 1: return 12 + py - (px * 4);  // 90
    case 2: return 15 - (py * 4) - px;  // 180
    case 3: return 3 - py + (px * 4);   // 270
  }
  return 0;
}


/* Gives the Tetromino identification number, its rotation, and the coordinates
** of the top-left angle w.r.t. the game field (nPosX, nPosY) determine
** if the piece can be inserted there or not */
bool DoesPieceFit (int nTetromino, int nRotation, int nPosX, int nPosY)
{
   /* Explore the full tetromino */
  for (int px = 0; px < 4; ++px)
    for (int py = 0; py < 4; ++py)
    {
      /* Get the local index in the piece */
      int pi = Rotate(px, py, nRotation);
      /* Geth the tetromino index w.r.t tje global game field */
      int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

      if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
      {
        if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
        {
          if (tetromino[nTetromino][pi] != '.' && pField[fi] != 0)
            return false;
        }
      }
    }
  return true;
} 



int main() {
  /* Start a ncurses sreen */
  initscr();
  refresh();
  getch();
  /* Copy the width and deph of the windows into these two variables,
   * easier to remember */
  getmaxyx(stdscr, nScreenHeight, nScreenWidth);

  /* Start the graphical session by using ncurses */
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  bkgd(COLOR_PAIR(1));
  addstr("Welcome to Tetris v.0.1! Press a key to start");
  curs_set(0);
  getch();
  refresh();

  /* Initialize tetromimo as the Tetris pieces */
  tetromino[0].append("..X.");  
  tetromino[0].append("..X.");  
  tetromino[0].append("..X.");  
  tetromino[0].append("..X.");  

  tetromino[1].append("..X.");  
  tetromino[1].append(".XX.");  
  tetromino[1].append(".X..");  
  tetromino[1].append("....");  

  tetromino[2].append(".X..");  
  tetromino[2].append(".XX.");  
  tetromino[2].append("..X.");  
  tetromino[2].append("....");  

  tetromino[3].append("....");  
  tetromino[3].append(".XX.");  
  tetromino[3].append(".XX.");  
  tetromino[3].append("....");  

  tetromino[4].append("..X.");  
  tetromino[4].append(".XX.");  
  tetromino[4].append("..X.");  
  tetromino[4].append("....");  

  tetromino[5].append(".X..");  
  tetromino[5].append(".X..");  
  tetromino[5].append(".XX.");  
  tetromino[5].append("....");  

  tetromino[6].append("..X.");  
  tetromino[6].append("..X.");  
  tetromino[6].append(".XX.");  
  tetromino[6].append("....");  

  /* Initialize the play field: always zero, except 9 on the boundary */
  pField = new unsigned char[nFieldWidth * nFieldHeight];
  for (int x = 0; x < nFieldWidth; ++x)
    for (int y = 0; y < nFieldHeight; ++y)
      pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 ||
                                     y == nFieldHeight - 1) ? 9 : 0;
  /* The play field is here not diaplayed: for the moment, just
   * initialized. It will be modified during the game loop, and printed
   * on the screen at the end of every game loop */

  /* Game logic variables */
  bool bGameOver = false;
  int nCurrPiece = 2;
  int nCurrRotation = 0;
  int nCurrX = nFieldWidth / 2;
  int nCurrY = 0;

  int ch;
  /* 0 = down, 1 = right, 2 = left, 3 = rotate */
  bool bKey[4];
  /* Number of game ticks to wait before moving down the piece */
  int nSpeed = 20;
  int nSpeedCounter = 0;
  bool bForceDown = false;
  int nPieceCount = 0;
  int nScore = 0;

  vector<int> vLines;

  /* During the game loop, getch() does not interrupt the input flow */
  nodelay(stdscr, TRUE);
  /* Start the game loop: core part of the game */
  while (!bGameOver)
  {
    // GAME TIMING
    napms(50);
    ++nSpeedCounter;
    bForceDown = (nSpeedCounter == nSpeed);
    mvaddch(1, 0, '0' + nSpeedCounter); 


    // INPUT
    /* Reset the keys */
    for (int k = 0; k < 4; ++k)
      bKey[k] = false;
    /* Read new keys */
    /* They jey pressed do not print on the screen */
    noecho();
    ch = getch();
    switch(ch)
    {
      case 'd':
        bKey[0] = true;
        break;
      case 'f':
        bKey[1] = true;
        break;
      case 's':
        bKey[2] = true;
        break;
      case ' ':
        bKey[3] = true;
        break;
      case 'q':
        bGameOver = true;
      default:
        break;
    }


    // GAME LOGIC
    /* Move the piece downward */
    if (bKey[0] && DoesPieceFit(nCurrPiece, nCurrRotation, nCurrX, nCurrY + 1))
        nCurrY++;
    /* Right key */
    if (bKey[1] && DoesPieceFit(nCurrPiece, nCurrRotation, nCurrX + 1, nCurrY))
        nCurrX++;
    /* Lefy Key */
    if (bKey[2] && DoesPieceFit(nCurrPiece, nCurrRotation, nCurrX - 1, nCurrY))
        nCurrX--;
    if (bKey[3] && DoesPieceFit(nCurrPiece, (nCurrRotation + 1) % 4,
                                                               nCurrX, nCurrY))
        nCurrRotation = (nCurrRotation + 1) % 4;


    /* Check if the piece must go down due to timeclock */
    if (bForceDown)
    {
      if(DoesPieceFit(nCurrPiece, nCurrRotation, nCurrX, nCurrY + 1))
        nCurrY++; 
      else
      {
        /* Lock the current piece in the field: i.e: copy it in background */
        for (int px = 0; px < 4; px++)
          for (int py = 0; py < 4; py++)
            if (tetromino[nCurrPiece][Rotate(px, py, nCurrRotation)] == 'X')
              pField[(nCurrY+py) * nFieldWidth + nCurrX + px] = nCurrPiece+1;
  
        /* Increase the game difficult every 10 pieces */
        nPieceCount++;
        if (nPieceCount % 10 == 0)
          if (nSpeed >= 10)
            nSpeed--;


        /* Check if we got any line */
        for (int py = 0; py < 4; py++)
        {
          if (nCurrY + py < nFieldHeight - 1)
          {
            bool bLine = true;
            for (int px = 1; px < nFieldWidth - 1; px++)
              bLine &= (pField[(nCurrY + py) * nFieldWidth + px]) != 0;
            /* if the line is full, bLine will keep the value true */
            /* in such a case, fill the entire line with the symbol '=' */  
            if (bLine)
            {
              for (int px = 1; px < nFieldWidth - 1; px++)
                pField[(nCurrY + py) * nFieldWidth + px] = 8;
              
              /* If a line exists, store its position into a vector */
              vLines.push_back(nCurrY + py);
      
            }
          }
        }
        

        /* Choose next piece */
        nCurrX = nFieldWidth / 2;
        nCurrY = 0;
        nCurrRotation = 0;
        nCurrPiece = rand() % 7;

        /* If piece does not fit: game over */
        bGameOver = !DoesPieceFit(nCurrPiece, nCurrRotation, nCurrX, nCurrY);
        if(bGameOver)
          mvaddstr(nScreenHeight - 3, 0, "COLLISION!");
      }
      nSpeedCounter = 0;
    }


    // RENDER OUTPUT
    /* Draw the game field */
    for (int x = 0; x < nFieldWidth; ++x)
      for (int y = 0; y < nFieldHeight; ++y)
        mvaddch(y + 2, x + 2, " ABCDEFG=#"[pField[y * nFieldWidth + x]]);

    /* Draw the Curr Piece */
    for (int px = 0; px < 4; ++px)
      for (int py = 0; py < 4; ++py)
        if (tetromino[nCurrPiece][Rotate(px, py, nCurrRotation)] != '.')
          mvaddch(nCurrY + 2 + py, nCurrX + 2 + px, 'A' + nCurrPiece);

    /* Pause a bit if we have got some lines */
    if (!vLines.empty())
    {
      nScore += (1 << vLines.size()) * 100;
      /* Pause a bit to give the user better experience */
      napms(400);
      /* For each row listed as full, delete and scroll the screen */ 
      for (auto &v : vLines)
      {
        for (int px = 1; px < nFieldWidth - 1; ++px)
        {
          for (int py = v; py > 0; py--)
            pField[py * nFieldWidth + px] = pField[(py-1) * nFieldWidth + px];
          pField[px] = 0;
        }
      }

      vLines.clear();

    }
    
    /* Empty the keyboard buffer */
    flushinp();  
    /* Refresh the ncurses window */
    move(nScreenHeight - 2, 0);
    deleteln();
    printw("SCORE: %d\tDIFFICULTY LEVEL: %d", nScore, 20 - nSpeed);
    refresh();
  }


  /* Display the screen and wait before closing */
  nodelay(stdscr, FALSE);
  mvaddstr(nScreenHeight - 1, 0, "GAME OVER. Exiting...");
  refresh();
  getch();
  endwin();
  return 0;
}
