#include "match3model.h"

Match3Model::Match3Model(QObject *parent, const int dimentionX, const int dimentionY)
    : QAbstractListModel(parent), moveCounter(0),  score(0), selectedSellIndex(-1), dimentionX(dimentionX), dimentionY(dimentionY)
{
 // get settings content (add exception ?^) )
    srand (time(NULL));

    initByJson();
    resetGame();
}

int Match3Model::rowCount(const QModelIndex &) const
{
    return dimentionX * dimentionY; // data reprented by 2d array
}

QVariant Match3Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int col =  index.row() / dimentionY;
    int row = index.row() % dimentionY;

    if (role == Qt::DecorationRole) {
        QString colorName = cellTypes[ cells[col][row] ].toString();
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
    return dimentionX;
}

int Match3Model::getDimentionY() const
{
    return dimentionY;
}

int Match3Model::getScore() const
{
    return score;
}

int Match3Model::getMoveCounter() const
{
    return moveCounter;
}

void Match3Model::resetGame()
{
        beginResetModel();

        cells.clear();
        generateCells();

        endResetModel();

        score = 0;
        emit scoreChanged();

        moveCounter = 0;
        emit moveCounterChanged();

        selectedSellIndex = -1;
}


void Match3Model::initByJson()
{
      Q_INIT_RESOURCE(qml);

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
      dimentionY = board["height"].toString().toUInt();
      dimentionX = board["width"].toString().toUInt();
      // qWarning() << board["height"].toInt(); // -> 0
      // set cells types
      cellTypes = jsonObject.value(QString("itemTypes")).toArray();
}

void Match3Model::generateCells()
{
    bool dataChanged = true;
    int multiplicator = 0;

    beginResetModel();
    cells.clear();

    for (int col = 0; col < dimentionX; ++col) {
        deque<int> tmp_vector;

        for (int row = 0; row < dimentionY; ++row) {
            tmp_vector.push_back(getRandomCellColorId());
        }

        cells.push_back(tmp_vector);
    }

    while (dataChanged) {
        qWarning() << "check cells";
        dataChanged = checkBoardForMatch(multiplicator);
    }

    endResetModel();
}


bool Match3Model::checkBoardForMatch(int &multiplicator)
{
    bool cellsChanged = false;
    for (int col = 0; col < dimentionX; ++col) {
        for (int row = 0; row < dimentionY; ++row) {
            cellsChanged |= findAndRemoveCellSets(col, row, multiplicator);
        }
    }

    return cellsChanged;
}

bool Match3Model::findAndRemoveCellSets(int x, int y, int &addToScore)
{
    queue<pair<int, int>> checkQuery;
    int matchArray[dimentionX * dimentionY];
    fill_n(matchArray, dimentionX * dimentionY, 0);

    checkQuery.push(pair<int, int>(x, y));

    while (!checkQuery.empty()) {
        int checkedX = checkQuery.front().first;
        int checkedY = checkQuery.front().second;
        if (cells[x][y] == cells[checkedX][checkedY] && !matchArray[checkedX * dimentionY + checkedY]) {
            matchArray[checkedX * dimentionY + checkedY] = 1;

            if (checkedX > 0) {
                checkQuery.push(pair<int, int>(checkedX - 1, checkedY));
            }
            if (checkedX < dimentionX - 1) {
                checkQuery.push(pair<int, int>(checkedX + 1, checkedY));
            }
            if (checkedY > 0) {
                checkQuery.push(pair<int, int>(checkedX, checkedY - 1));
            }
            if (checkedY < dimentionY - 1) {
                checkQuery.push(pair<int, int>(checkedX, checkedY + 1));
            }
        }
        checkQuery.pop();
    }

    //delete if match3
    return removeCellsIfNedded(matchArray, addToScore);
}

bool Match3Model::removeCellsIfNedded(int *boardCells, int &addToScore)
{
    bool shuoldBeDeleted = false;
    int elementsMatch;
    // check rows
    for (int col = 0; col < dimentionX; ++col) {
        elementsMatch  = 0;
        for (int row = 0; row < dimentionY; ++row) {
            if (boardCells[col * dimentionY + row]) {
                elementsMatch++;

                if (elementsMatch >= 3) {
                    shuoldBeDeleted = true;
                    // to exit
                    col = dimentionX;
                    row = dimentionY;
                }
            }
        }
    }

    if (!shuoldBeDeleted ) {
        // check cols
        for (int row = 0; row < dimentionY; ++row) {
            elementsMatch  = 0;
            for (int col = 0; col < dimentionX; ++col) {
               if (boardCells[col * dimentionY + row]) {
                    elementsMatch++;

                    if (elementsMatch >= 3) {
                        shuoldBeDeleted = true;
                        // to exit
                        col = dimentionX;
                        row = dimentionY;
                    }
                }
            }
        }
    }

    if (shuoldBeDeleted) {
        removeCells(boardCells, addToScore);
        addToScore++;
    }

    return shuoldBeDeleted;
}

void Match3Model::removeCells(int *boardCells, int addToScore)
{
    for (int col = 0; col < dimentionX; ++col) {
        for (int row = 0; row < dimentionY; ++row) {
          if (boardCells[col * dimentionY + row]) {
                removeElement(col, row, addToScore);
            }
        }
    }
}

void Match3Model::removeElement(int col, int row, int addToScore)
{
    int index = col * dimentionY + row;
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();

    qWarning() << "deleted: " <<cellTypes[ cells[col][row]];
    cells[col].erase(cells[col].begin() + row);

    beginInsertRows(QModelIndex(), col * dimentionY, col * dimentionY);
    cells[col].push_front(getRandomCellColorId());
    endInsertRows();

    increaseScore(addToScore);
//    auto newIndex = createIndex(index,0);
//    emit dataChanged(newIndex, newIndex, {Qt::DecorationRole});

}

int Match3Model::getRandomCellColorId()
{
    return rand() % cellTypes.size();
}

void Match3Model::increaseScore(int multiplicator)
{
    score += 10 + 2 * multiplicator;
    emit scoreChanged();
}

void Match3Model::increaseMoveCounter()
{
    moveCounter++;
    emit moveCounterChanged();
}

bool Match3Model::swapElements(int sourceIndex, int targetIndex, bool checkResult)
{
    int sourceCol =  sourceIndex/ dimentionY;
    int sourceRow = sourceIndex % dimentionY;

    int targetCol =  targetIndex/ dimentionY;
    int targetRow = targetIndex % dimentionY;

    int shift = sourceIndex > targetIndex ? 0 : 1;
    int multiplicator = 0;

    beginMoveRows(QModelIndex(), sourceIndex, sourceIndex, QModelIndex(), targetIndex + shift);
    endMoveRows();

    if (abs(sourceIndex - targetIndex) > 1) {
        qWarning() << "if";
        int zeroPositionShift = targetIndex > sourceIndex ? -1 : 1;
        beginMoveRows(QModelIndex(), targetIndex + zeroPositionShift, targetIndex + zeroPositionShift,QModelIndex(), sourceIndex + shift + zeroPositionShift);
        endMoveRows();
    }

    swap(cells[sourceCol][sourceRow], cells[targetCol][targetRow]);
    if (checkResult) {
        checkResult = findAndRemoveCellSets(sourceCol, sourceRow, multiplicator) || findAndRemoveCellSets(targetCol, targetRow, multiplicator );
        if (checkResult) {
            while (checkResult) {
                qWarning() << "check cells";
                checkResult = checkBoardForMatch(multiplicator);
            }
            return !checkResult;
        } else {
            return checkResult;
        }
    }
//    return true;
    return false;
}

bool Match3Model::choseElement(int index)
{
    if (selectedSellIndex >= 0 && selectedSellIndex != index) {
        // swap elements
        if (swapElements(selectedSellIndex, index)) {
            increaseMoveCounter();
            selectedSellIndex = -1;
        } else {
            selectedSellIndex = index;
            return true;
        }

        return false;
    } else {
        selectedSellIndex = index;
    }

    return false;
}

void Match3Model::reSwapElements(int index)
{
    swapElements(selectedSellIndex, index, false);
    selectedSellIndex = -1;
}

