
#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <iostream>
#include <vector>

using std::vector;

class Person {

public:
  int id, row, col;
  bool alive;
  Person *next, *prev;
  Person(int id, int row, int col) {
    this->id = id;
    this->row = row;
    this->col = col;
    this->alive = true;
    this->next = this->prev = nullptr;
  }
};

class District {

public:
  int r, c, pop;
  Person *head, *tail;
  District(int r, int c) {
    this->r = r;
    this->c = c;
    this->pop = 0;
    this->head = this->tail = nullptr;
  }
};

class GridWorld {

private:
  int ncols, nrows, pop;
  vector<vector<District*> > grid;
  vector<Person *> populationn;
  vector<int> recentlyDead;

public:
  /**
   * constructor:  initializes a "world" with nrows and
   *    ncols (nrows*ncols districtcs) in which all
   *    districtricts are empty (a wasteland!).
   */
  GridWorld(unsigned nrows, unsigned ncols) {
    this->nrows = nrows;
    this->ncols = ncols;
    this->pop = 0;
    this->grid.resize(nrows, vector<District *>(ncols, nullptr));
    for (int r = 0; r < nrows; r++) {
      for (int c = 0; c < nrows; c++) {
        this->grid[r][c] = new District(r, c);
      }
    }
  }

  ~GridWorld() {
    for (Person *person : populationn) {
      delete person;
    }
    for (int r = 0; r < nrows; r++) {
      for (int c = 0; c < nrows; c++) {
        delete grid[r][c];
      }
    }
  }

  /*
   * function: birth
   * description:  if row/col is valid, a new person is created
   *   with an ID according to rules in handout.  New person is
   *   placed in district (row, col)
   *
   * return:  indicates success/failure
   */
  bool birth(int row, int col, int &id) {
    if (row >= 0 && row < nrows && col >= 0 && col < ncols) {
      District *district = grid[row][col];
      Person *person;
      if (recentlyDead.empty()) {
        id = this->pop;
        person = new Person(id, row, col);
        populationn.push_back(person);
      } else {
        id = recentlyDead.front();
        recentlyDead.erase(recentlyDead.begin());
        person = populationn[id];
        person->alive = true;
        person->row = row;
        person->col = col;
      }
      if (district->head) {
        district->tail->next = person;
        person->prev = district->tail;
        district->tail = person;
      } else {
        district->head = district->tail = person;
      }
      district->pop++;
      this->pop++;
      return true;
    }
    return false;
  }

  /*
   * function: death
   * description:  if given person is alive, person is killed and
   *   data structures updated to reflect this change.
   *
   * return:  indicates success/failure
   */
  bool death(int personID) {
    if (personID >= 0 && personID < populationn.size() &&
        populationn[personID]) {
      Person *person = populationn[personID];
      District *district = grid[person->row][person->col];
      if (district->head == district->tail) {
        district->head = district->tail = nullptr;
      } else if (district->head == person) {
        district->head = district->head->next;
        district->head->prev = nullptr;
      } else if (district->tail == person) {
        district->tail = district->tail->prev;
        district->tail->next = nullptr;
      } else {
        person->prev->next = person->next;
        person->next->prev = person->prev;
      }
      district->pop--;
      this->pop--;
      person->alive = false;
      person->next = person->prev = nullptr;
      recentlyDead.push_back(person->id);
      return true;
    }
    return false;
  }

  /*
   * function: whereis
   * description:  if given person is alive, his/her current residence
   *   is reported via reference parameters row and col.
   *
   * return:  indicates success/failure
   */
  bool whereis(int id, int &row, int &col) const {
    if (id >= 0 && id <= populationn.size() && populationn[id]) {
      Person *person = populationn[id];
      if (person->alive == true) {
        row = person->row;
        col = person->col;
        return true;
      }
    }
    return false;
  }

  /*
   * function: move
   * description:  if given person is alive, and specified target-row
   *   and column are valid, person is moved to specified district and
   *   data structures updated accordingly.
   *
   * return:  indicates success/failure
   *
   * comment/note:  the specified person becomes the 'newest' member
   *   of target district (least seniority) --  see requirements of members().
   */
  bool move(int id, int targetRow, int targetCol) {
    if (targetRow >= 0 && targetRow < nrows && targetCol >= 0 && targetCol < ncols && populationn[id]->alive) {
      death(id);
      birth(targetRow, targetCol, id);
      return true;
    }
    return false;
  }

  std::vector<int> *members(int row, int col) const {
    vector<int> *residents = new std::vector<int>;
    if (row >= 0 && row < nrows && col >= 0 && col < ncols) {
      District *district = grid[row][col];
      Person *person = district->head;
      while (person != nullptr) {
        residents->push_back(person->id);
        person = person->next;
      }
    }
    return residents;
  }

  /*
   * function: population
   * description:  returns the current (living) population of the world.
   */
  int population() const { return this->pop; }

  /*
   * function: population(int,int)
   * description:  returns the current (living) population of specified
   *   district.  If district does not exist, zero is returned
   */
  int population(int row, int col) const {
    if (row >= 0 && row < nrows && col >= 0 && col < ncols) {
      District *district = grid[row][col];
      return district->pop;
    }
    return 0;
  }

  /*
   * function: num_rows
   * description:  returns number of rows in world
   */
  int num_rows() const { return this->nrows; }

  /*
   * function: num_cols
   * description:  returns number of columns in world
   */
  int num_cols() const { return this->ncols; }
};

#endif