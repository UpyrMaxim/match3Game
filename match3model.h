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
#include <array>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>


using namespace std;
class Match3Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int dimentionX  READ getDimentionX  NOTIFY dimentionXChanged)
    Q_PROPERTY(int dimentionY  READ getDimentionY  NOTIFY dimentionYChanged)
    Q_PROPERTY(int moveCounter READ getScore       NOTIFY scoreChanged)
    Q_PROPERTY(int score       READ getMoveCounter NOTIFY moveCounterChanged)

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
    void increaseScore();
    void increaseMoveCounter();
    // match methods
    bool checkBoardForMatch();
    bool checkCell(int x, int y, bool addToScore = false);
    bool removeCellsIfNedded(int *boardCells, bool addToScore);
    void removeCells(int *boardCells, bool addToScore);
    void removeElement(int col, int row, bool addToScore);
private:
    // board data
   deque<deque<int> > cells;
   int moveCounter;
   int score;
   // setting
   QJsonArray cellTypes;
   mutable int dimentionX;
   mutable int dimentionY;
};



#endif // Match3Model_H
