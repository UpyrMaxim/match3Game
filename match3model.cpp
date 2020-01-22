#include "match3model.h"

Match3Model::Match3Model(QObject *parent, const int dimentionX, const int dimentionY)
    : QAbstractListModel(parent), m_moveCounter(0),  m_score(0), m_selectedSellIndex(-1), m_dimentionX(dimentionX), m_dimentionY(dimentionY)
{
    srand (time(NULL));

    initByJson();
    resetGame();
}

int Match3Model::rowCount(const QModelIndex &) const
{
    return m_dimentionX * m_dimentionY; // data reprented by 2d array
}

QVariant Match3Model::data(const QModelIndex &index, int role) const
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

Qt::ItemFlags Match3Model::flags(const QModelIndex &index) const
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
        generateCells();
        removeAllMatches();

        m_score = 0;
        emit scoreChanged();

        m_moveCounter = 0;
        emit moveCounterChanged();

        m_selectedSellIndex = -1;
}


void Match3Model::initByJson()
{
      QString val;
      QFile file;
      file.setFileName(":/setting.json");
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          qWarning() << "Setting file is not exists";
          return;
      }
      val = file.readAll();
      file.close();

      // get json content
      QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
      QJsonObject jsonObject = document.object();
      QJsonObject board = jsonObject.value(QString("board")).toObject();

      // set dimention values
      m_dimentionY = board["height"].toInt();
      m_dimentionX = board["width"].toInt();
      // set cells types
      m_cellTypes = jsonObject.value(QString("itemTypes")).toArray();
}

void Match3Model::generateCells()
{
    m_cells.clear();

    for (int col = 0; col < m_dimentionX; ++col) {
        deque<int> tmp_vector;

        for (int row = 0; row < m_dimentionY; ++row) {
            tmp_vector.push_back(getRandomCellColorId());
        }

        m_cells.push_back(tmp_vector);
    }
}


void Match3Model::removeElement(int col, int row, int addToScore)
{
    int index = col * m_dimentionY + row;
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();

    m_cells[col].erase(m_cells[col].begin() + row);

    beginInsertRows(QModelIndex(), col * m_dimentionY, col * m_dimentionY);
    m_cells[col].push_front(getRandomCellColorId());
    endInsertRows();

    increaseScore(addToScore);
//    auto newIndex = createIndex(index,0);
//    emit dataChanged(newIndex, newIndex, {Qt::DecorationRole});

}

void Match3Model::removeAllMatches()
{
    beginResetModel();

    bool dataChanged = true;
    while (dataChanged) {
        auto cellsToRemove = checkBoard();
        if (cellsToRemove.size()) {
            for (const auto &index : cellsToRemove) {
                int col =  index / m_dimentionY;
                int row = index % m_dimentionY;
                removeElement(col, row, 0);
            }
        } else {
            dataChanged = false;
        }
    }

    endResetModel();
}

int Match3Model::getRandomCellColorId()
{
    return rand() % m_cellTypes.size();
}

void Match3Model::increaseScore(int multiplicator)
{
    m_score += 10 + 2 * multiplicator;
    emit scoreChanged();
}

void Match3Model::increaseMoveCounter()
{
    m_moveCounter++;
    emit moveCounterChanged();
}

QList<int> Match3Model::swapCells(int sourceIndex, int targetIndex, bool reSwap)
{
    if (sourceIndex == targetIndex || sourceIndex < 0 || targetIndex < 0) {
        return QList<int>();
    }

    int sourceCol =  sourceIndex / m_dimentionY;
    int sourceRow = sourceIndex % m_dimentionY;

    int targetCol =  targetIndex / m_dimentionY;
    int targetRow = targetIndex % m_dimentionY;

    int shift = sourceIndex > targetIndex ? 0 : 1;

    beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), targetIndex + shift);
    endMoveRows();

    if (abs(sourceIndex - targetIndex) > 1) {
        int zeroPositionShift = targetIndex > sourceIndex ? -1 : 1;
        beginMoveRows(QModelIndex(), targetIndex + zeroPositionShift, targetIndex + zeroPositionShift, QModelIndex(), sourceIndex + shift + zeroPositionShift);
        endMoveRows();
    }

    swap(m_cells[sourceCol][sourceRow], m_cells[targetCol][targetRow]);
    if (reSwap) {
        return QList<int>();
    }

    increaseMoveCounter();
    return findCellsToRemove(sourceCol, sourceRow) + findCellsToRemove(targetCol, targetRow);
}


QList<int> Match3Model::findCellsToRemove(int x, int y)
{
    queue<pair<int, int>> checkQuery;
    int matchArray[m_dimentionX * m_dimentionY];
    fill_n(matchArray, m_dimentionX * m_dimentionY, 0);

    checkQuery.push(pair<int, int>(x, y));

    while (!checkQuery.empty()) {
        int checkedX = checkQuery.front().first;
        int checkedY = checkQuery.front().second;
        if (m_cells[x][y] == m_cells[checkedX][checkedY] && !matchArray[checkedX * m_dimentionY + checkedY]) {
            matchArray[checkedX * m_dimentionY + checkedY] = 1;

            if (checkedX > 0) {
                checkQuery.push(pair<int, int>(checkedX - 1, checkedY));
            }
            if (checkedX < m_dimentionX - 1) {
                checkQuery.push(pair<int, int>(checkedX + 1, checkedY));
            }
            if (checkedY > 0) {
                checkQuery.push(pair<int, int>(checkedX, checkedY - 1));
            }
            if (checkedY < m_dimentionY - 1) {
                checkQuery.push(pair<int, int>(checkedX, checkedY + 1));
            }
        }
        checkQuery.pop();
    }

    return findMatch3Items(matchArray);
}


QList<int> Match3Model::findMatch3Items(int *boardCells)
{
    bool shuoldBeDeleted = false;
    int elementsMatch;
    QList<int> cellsToRemove;
    // check rows
    for (int col = 0; col < m_dimentionX; ++col) {
        elementsMatch  = 0;
        for (int row = 0; row < m_dimentionY; ++row) {
            int cellIndex = col * m_dimentionY + row;
            if (boardCells[cellIndex]) {
                elementsMatch++;
                cellsToRemove.push_back(cellIndex);
                if (elementsMatch >= 3) {
                    shuoldBeDeleted = true;
                }
            }
        }
    }

    if (!shuoldBeDeleted ) {
        // check cols
        for (int row = 0; row < m_dimentionY; ++row) {
            elementsMatch  = 0;
            for (int col = 0; col < m_dimentionX; ++col) {
               if (boardCells[col * m_dimentionY + row]) {
                    elementsMatch++;

                    if (elementsMatch >= 3) {
                        shuoldBeDeleted = true;
                        // to exit
                        col = m_dimentionX;
                        row = m_dimentionY;
                    }
                }
            }
        }
    }

    if (shuoldBeDeleted) {
        return cellsToRemove;
    }

    return QList<int>();
}



QList<int> Match3Model::reSwapCells(int sourceIndex, int targetIndex)
{
    return swapCells(sourceIndex, targetIndex, true);
}

void Match3Model::removeCell(int index)
{
    int col =  index / m_dimentionY;
    int row = index % m_dimentionY;

    removeElement(col, row, 0);
}

QList<int> Match3Model::checkBoard()
{
    QList<int> cellsList;
    for (int col = 0; col < m_dimentionX; ++col) {
        for (int row = 0; row < m_dimentionY; ++row) {
            cellsList += findCellsToRemove(col, row);
        }
    }

    return cellsList.toSet().toList(); // its make it unique
}
