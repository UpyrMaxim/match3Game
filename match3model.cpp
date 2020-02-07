#include "match3model.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include <QColor>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

Match3Model::Match3Model(QObject * parent, const int dimentionX, const int dimentionY)
    : QAbstractListModel(parent), m_moveCounter(0),  m_score(0),  m_dimentionX(dimentionX), m_dimentionY(dimentionY)
{
    srand(time(nullptr));

    initByJson();
    resetGame();
}

int Match3Model::rowCount(const QModelIndex &) const
{
    return m_dimentionX * m_dimentionY; // data reprented by 2d array
}

QVariant Match3Model::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int col = index.row() / m_dimentionY;
    int row = index.row() % m_dimentionY;

    if (role == Qt::DecorationRole) {
        QString colorName = m_cellTypes[ m_cells[col][row] ].toString();
        return QColor(colorName);
    } else {
        return QVariant();
    }
}

Qt::ItemFlags Match3Model::flags(const QModelIndex & index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int Match3Model::getDimentionX() const
{
    return m_dimentionX;
}

int Match3Model::getDimentionY() const
{
    return m_dimentionY;
}

int Match3Model::getScore() const
{
    return m_score;
}

int Match3Model::getMoveCounter() const
{
    return m_moveCounter;
}

void Match3Model::resetGame()
{
    beginResetModel();

    // moves must be available after resetting
    do {
        generateCells();
    } while (!checkGameOver());

    m_score = 0;
    emit scoreChanged();

    m_moveCounter = 0;
    emit moveCounterChanged();

    endResetModel();
}

void Match3Model::initByJson()
{
    QFile file(":/setting.json");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Setting file is not exists";
        return;
    }

    QString val = file.readAll();
    file.close();

    // get json content
    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObject = document.object();
    QJsonObject board = jsonObject.value(QString("board")).toObject();

    // set dimention values
    m_dimentionY = minMatch > board["height"].toInt() ? minMatch : board["height"].toInt();
    m_dimentionX = minMatch > board["width"].toInt() ?  minMatch : board["width"].toInt();
    // set cells types
    m_cellTypes = jsonObject.value(QString("itemTypes")).toArray();
}

void Match3Model::generateCells()
{
    m_cells.clear();

    for (int col = 0; col < m_dimentionX; ++col) {
        QList<int> tmp_vector;

        for (int row = 0; row < m_dimentionY; ++row) {
            tmp_vector.push_back(getRandomCellColorId());
        }

        m_cells.push_back(tmp_vector);
    }

    removeAllMatches();
}

void Match3Model::removeElements(const QList<int> & matches)
{
    if (!matches.size()) {
        return;
    }

    int col =  matches.front() / m_dimentionY;
    int row =  matches.front() % m_dimentionY;

    beginRemoveRows(QModelIndex(), matches.front(), matches.front() + matches.size() - 1);

    for (int i = 0; i < matches.size(); ++i) {
        m_cells[col].removeAt(row);
    }

    endRemoveRows();

    beginInsertRows(QModelIndex(), col * m_dimentionY, col * m_dimentionY + matches.size() -1);

    for (int i = 0; i < matches.size(); ++i) {
        m_cells[col].push_front(getRandomCellColorId());
    }

    endInsertRows();
    increaseScore(matches.size());
}

void Match3Model::removeMatches()
{
    int prevCol = -1;
    int prevRow = -1;
    QList<int> indexesToRemove;

    for (int i = 0; i <= m_cellsToRemove.size(); ++i) {
        if(i == m_cellsToRemove.size()) {
             removeElements(indexesToRemove);
        } else {
            int col = m_cellsToRemove[i] / m_dimentionY;
            int row = m_cellsToRemove[i] % m_dimentionY;

            if (col == prevCol && abs(prevRow - row) == 1) {
                indexesToRemove.push_back(m_cellsToRemove[i]);
                 prevRow = row;
            } else {
                removeElements(indexesToRemove);
                indexesToRemove.clear();
                prevCol = col;
                prevRow = row;
                indexesToRemove.push_back(m_cellsToRemove[i]);
            }
        }
    }

    m_cellsToRemove.clear();
}

void Match3Model::removeAllMatches()
{
    bool dataChanged = true;

    while (dataChanged) {
        checkBoardCells();

        if (m_cellsToRemove.size()) {
            removeMatches();
        } else {
            dataChanged = false;
        }
    }
}

int Match3Model::getRandomCellColorId()
{
    return rand() % m_cellTypes.size();
}

void Match3Model::increaseScore(int multiplicator)
{
    m_score += 10 * multiplicator;
    emit scoreChanged();
}

void Match3Model::increaseMoveCounter()
{
    m_moveCounter++;
    emit moveCounterChanged();
}

bool Match3Model::chooseCell(int sourceIndex, int targetIndex)
{
    int indexesDifference = abs(sourceIndex - targetIndex);
    bool swapIsValid = indexesDifference == 1 || indexesDifference == m_dimentionY;

    if (!swapIsValid) {
        return false;
    }

    int sourceCol =  sourceIndex / m_dimentionY;
    int sourceRow = sourceIndex % m_dimentionY;
    int targetCol =  targetIndex / m_dimentionY;
    int targetRow = targetIndex % m_dimentionY;

    swap(m_cells[sourceCol][sourceRow], m_cells[targetCol][targetRow]);
    checkBoardCells();

    if (!m_cellsToRemove.size()) {
        swap(m_cells[sourceCol][sourceRow], m_cells[targetCol][targetRow]);
        return false;
    }

    moveCells(sourceIndex, targetIndex);
    increaseMoveCounter();

    return true;
}

void Match3Model::moveCells(int sourceIndex, int targetIndex)
{
    int shift = targetIndex > sourceIndex ? 0 : 1;

    beginMoveRows(QModelIndex(), targetIndex, targetIndex, QModelIndex(), sourceIndex + shift);
    endMoveRows();

    if (abs(targetIndex - sourceIndex) > 1) {
        int zeroPositionShift = sourceIndex > targetIndex ? -1 : 1;
        beginMoveRows(QModelIndex(), sourceIndex + zeroPositionShift, sourceIndex + zeroPositionShift, QModelIndex(), targetIndex + shift + zeroPositionShift);
        endMoveRows();
    }
}

void Match3Model::actionCompleted()
{
    if (!m_cellsToRemove.size()) {
        if (!checkGameOver()) {
            emit gameOver();
        }

        return;
    }

    removeMatches();
    checkBoardCells();
}

void Match3Model::checkBoardCells()
{
    typedef QVector<int> Inner;
    QVector<Inner> checkRows(m_dimentionX, Inner(m_dimentionY));
    QVector<Inner> checkCols(m_dimentionX, Inner(m_dimentionY));

    for(int col = 0; col < m_dimentionX; ++col) {
        checkCol(checkCols, col, 0);
    }

    for(int row = 0; row < m_dimentionY; ++row) {
        checkRow(checkRows, 0, row);
    }

    for (int col = 0; col < m_dimentionX; ++col) {
        for (int row = 0; row < m_dimentionY; ++row) {
            if ( max(checkCols[col][row], checkRows[col][row]) >= minMatch ) {
                m_cellsToRemove.push_back(col * m_dimentionY + row);
            }
        }
    }
}

bool Match3Model::checkGameOver()
{
    // check by row
    for (int col = 0; col< m_dimentionX; ++col) {
        int prevVal = m_cells[col][0];

        for (int row = 1; row < m_dimentionY; ++row) {
            if (prevVal == m_cells[col][row]) {
                if(checkSideSwapRow(col, row, prevVal)) {
                    return true;
                }
            } else if (row < m_dimentionX - 1 && prevVal == m_cells[col][row + 1]) {
                if (checkMidSwapRow(col, row, prevVal)) {
                    return true;
                }
            }

            prevVal = m_cells[col][row];
        }
    }

    // check by column
    for (int row = 0; row < m_dimentionX; ++row) {
        int prevVal = m_cells[0][row];

        for (int col = 1; col < m_dimentionY; ++col) {
            if (prevVal == m_cells[col][row]) {
                if(checkSideSwapCol(col, row, prevVal)) {
                    return true;
                }
            }

            if (col < m_dimentionY - 1 && prevVal == m_cells[col + 1][row]) {
                if (checkMidSwapCol(col, row, prevVal)) {
                    return true;
                }
            }

            prevVal = m_cells[col][row];
        }
    }

    return false;
}

bool Match3Model::checkMidSwapRow(int col, int row, int value)
{
    bool swapable = false;

    if (col > 0) {
        swapable |= m_cells[col - 1][row] == value;
    }

    if (col < m_dimentionY - 1) {
        swapable |= m_cells[col + 1][row] == value;
    }

    return swapable;
}

bool Match3Model::checkMidSwapCol(int col, int row, int value)
{
    bool swapable = false;

    if (row > 0) {
        swapable |= m_cells[col][row - 1] == value;
    }

    if (row < m_dimentionX - 1) {
        swapable |= m_cells[col][row + 1] == value;
    }

    return swapable;
}

bool Match3Model::checkSideSwapCol(int col, int row, int value)
{
    bool swapable = false;

    if (col > 2) {
        swapable |= m_cells[col - 3][row] == value;
    }

    if (col > 1 && row > 0) {
        swapable |= m_cells[col - 2][row - 1] == value;
    }

    if (col > 1 && row < m_dimentionX - 1) {
        swapable |= m_cells[col - 2][row + 1] == value;
    }

    if (col < m_dimentionY - 1 && row > 0) {
        swapable |= m_cells[col + 1][row - 1] == value;
    }

    if (col < m_dimentionY - 1 && row < m_dimentionX - 1) {
        swapable |= m_cells[col + 1][row + 1] == value;
    }

    if (col < m_dimentionY - 2) {
        swapable |= m_cells[col + 2][row] == value;
    }

    return swapable;
}

bool Match3Model::checkSideSwapRow(int col, int row, int value)
{
    bool swapable = false;

    if (row > 2) {
        swapable |= m_cells[col][row - 3] == value;
    }

    if (col > 0 && row > 1) {
        swapable |= m_cells[col - 1][row - 2] == value;
    }

    if (col < m_dimentionY - 1 && row > 1) {
        swapable |= m_cells[col + 1][row - 2] == value;
    }

    if (col > 0 && row < m_dimentionX - 1) {
        swapable |= m_cells[col - 1][row + 1] == value;
    }

    if (col < m_dimentionY - 1 && row < m_dimentionX - 1) {
        swapable |= m_cells[col + 1][row + 1] == value;
    }

    if (row < m_dimentionX - 2) {
        swapable |= m_cells[col][row + 2] == value;
    }

    return swapable;
}

int Match3Model::checkCol(QVector<QVector<int>> & cells, int col, int row,int value)
{
    if (row == m_dimentionY - 1) {
        cells[col][row] = value;
        return value;
    }

    bool equal = m_cells[col][row] == m_cells[col][row + 1];
    int newValue = equal ? value + 1 : 1;
    int matches = checkCol(cells, col, row + 1, newValue);

    if (equal) {
        value = matches;
    }

    cells[col][row] = value;

    return value;
}

int Match3Model::checkRow(QVector<QVector<int>> & cells, int col, int row, int value)
{
    if (col == m_dimentionX - 1) {
        cells[col][row] = value;
        return value;
    }

    bool equal = m_cells[col][row] == m_cells[col + 1][row];
    int newValue = equal ? value + 1 : 1;
    int matches = checkRow(cells, col + 1, row, newValue);

    if (equal) {
        value = matches;
    }

    cells[col][row] = value;

    return value;
}
