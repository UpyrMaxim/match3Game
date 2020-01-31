#pragma once

#include <QAbstractListModel>
#include <QJsonArray>

using namespace std;
class Match3Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int dimentionX     READ getDimentionX  NOTIFY dimentionXChanged)
    Q_PROPERTY(int dimentionY     READ getDimentionY  NOTIFY dimentionYChanged)
    Q_PROPERTY(int moveCounter    READ getMoveCounter NOTIFY moveCounterChanged)
    Q_PROPERTY(int score          READ getScore       NOTIFY scoreChanged)

public:
    Match3Model(QObject * parent = nullptr,const int dimentionX = 6, const int dimentionY = 6);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    int getDimentionX() const;
    int getDimentionY() const;
    int getScore() const;
    int getMoveCounter() const;

    Q_INVOKABLE void resetGame();
    Q_INVOKABLE bool chooseCell(int sourceIndex, int targetIndex);

public slots:
    void removeCells();


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
    void removeElements(const QList<int> & matches);
    void removeMatches();
    void moveCells(int sourceIndex, int targetIndex);
    void removeAllMatches();
    int checkCol(QVector<QVector<int>> & cells, int col, int row, int value = 1);
    int checkRow(QVector<QVector<int>> & cells, int col, int row, int value = 1);
    void checkBoardCells();
private:
    const int minMatch = 3;

    QList<QList<int>> m_cells;
    int m_moveCounter;
    int m_score;
    // settings
    QList<int> m_cellsToRemove;
    QJsonArray m_cellTypes;
    int m_dimentionX;
    int m_dimentionY;
};
