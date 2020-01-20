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
    Q_INVOKABLE QList<int> swapCells(int sourceIndex, int targetIndex, bool reSwap = false);
    Q_INVOKABLE QList<int> reSwapCells(int sourceIndex, int targetIndex);
    Q_INVOKABLE void removeCell(int index);
    Q_INVOKABLE QList<int> checkBoard();

signals:
    void dimentionXChanged();
    void dimentionYChanged();
    void scoreChanged();
    void moveCounterChanged();

private:
    void initByJson();
    void generateCells();
    int getRandomCellColorId();
    void increaseScore(int multiplicator = 0);
    void increaseMoveCounter();
    void removeElement(int col, int row, int addToScore);

    QList<int> findCellsToRemove(int sourceCol, int sourceRow);
    QList<int> findMatch3Items(int *boardCells);
private:
   deque<deque<int> > m_cells;
   int m_moveCounter;
   int m_score;
   int m_selectedSellIndex;
   // settings
   QJsonArray m_cellTypes;
   int m_dimentionX;
   int m_dimentionY;
};



#endif // Match3Model_H
