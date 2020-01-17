#ifndef Match3Model_H
#define Match3Model_H

#include <QAbstractListModel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QJsonArray>
#include <QColor>

#include <QDebug>

#include <deque>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>


using namespace std;
class Match3Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int dimentionX  READ getDimentionX  NOTIFY dimentionXChanged)
    Q_PROPERTY(int dimentionY  READ getDimentionY  NOTIFY dimentionYChanged)
    Q_PROPERTY(int moveCounter READ getMoveCounter NOTIFY moveCounterChanged)
    Q_PROPERTY(int score       READ getScore       NOTIFY scoreChanged)

public:
    Match3Model(QObject *parent = nullptr,const int dimentionX = 6, const int dimentionY = 6);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int getDimentionX() const;
    int getDimentionY() const;
    int getScore() const;
    int getMoveCounter() const;

    Q_INVOKABLE void resetGame();
    Q_INVOKABLE bool choseElement(int index);
    Q_INVOKABLE void reSwapElements(int index);

signals:
    void dimentionXChanged();
    void dimentionYChanged();
    void scoreChanged();
    void moveCounterChanged();

//    Q_INVOKABLE void swapCells();
private:
    // generic methods
    void initByJson();
    void generateCells();
    int getRandomCellColorId();
    void increaseScore(int multiplicator = 0);
    void increaseMoveCounter();
    bool swapElements(int sourceIndex, int targetIndexe, bool checkResult = true);
    // match methods
    bool checkBoardForMatch(int &multiplicator);
    bool findAndRemoveCellSets(int x, int y, int &addToScore);
    bool removeCellsIfNedded(int *boardCells, int &addToScore);
    void removeCells(int *boardCells, int addToScore);
    void removeElement(int col, int row, int addToScore);
private:
    // board data
   deque<deque<int> > cells;
   int moveCounter;
   int score;
   int selectedSellIndex;
   // setting
   QJsonArray cellTypes;
   int dimentionX;
   int dimentionY;
};



#endif // Match3Model_H
