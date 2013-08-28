#include "world.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

// In the constructor of a world the list of people is generated.
World::World() {
	srand(time(NULL));
  master_list = (person *) malloc(NUM_PEOPLE * sizeof(person));
  master_count = generate_people(&master_list, NUM_PEOPLE);
  num_i = 0;
  num_s = master_count;
  num_r = 0;
  printf("master_count: %d\n", master_count);
  simtime = 0;

  int closest_index = 0;
  for (int i = 0; i < master_count; i++) {
    if(toroidal_distance(master_list[i].x, .5, master_list[i].y, .5) < toroidal_distance(master_list[closest_index].x, .5, master_list[closest_index].y, .5))
      closest_index = i;
  }
  infect(closest_index);
}

World::~World() {
  free(master_list);
}

// Returns all people in the world
World::person* World::get_people() {
  return master_list;
}

int World::get_count() {
  return master_count;
}


double World::linear_dist(double d) {
  return d;
}

double World::exp_dist(double d) {
  double u = rand()/((double)RAND_MAX + 1);

  double lambda = 62;
  double min = .5 / sqrt(master_count);
  double val = min - (1./lambda)*log(1-u);
  //printf("%f\n", val);
  return val;

  // exponential random variable
  int a = 1;
  int b = 2;
  int c = 3;
  double r = d * 60;
  return pow(a+r, -b) * exp(-r / c);
  //return exp(-EXP_PARAMETER * d);
}

double World::pow_dist(double d) {
  // uniform random variable in (0, 1)

  double alpha = 2;
  double min = .5 / sqrt(master_count);
  double u = rand()/((double)RAND_MAX + 1);
  double val = min * pow(1-u,-1./(alpha-1));
  return val;

  // power law random variable
  int a = 1;
  int b = 3;
  double r = d * 400;
  return pow(a+r, -b);
  //return pow(1 + pow(d*POW_R_0, POW_ALPHA), -1);
}

	
// Creates a list of n people for the world, currently uniformly
// randomly distributed on a (0, 1) torus.
int World::generate_people(person **all_people, int n) {
  //int count = populate_people(*all_people, n);
  printf("generating people\n");
	int i;
	for(i = 0; i < n; i++) {
		populate_person_grid(&((*all_people)[i]), i, (int)sqrt(n), (int)sqrt(n));
	}
  printf("done generating people\n");
  
	return NUM_PEOPLE;//count;
}

// Infect the person in master_list at given index and update counters
int World::infect(int index) {
  //many times we will try to infect somebody who is already infected.
  //in this case do nothing.
  if(master_list[index].status == SUSCEPTIBLE) {
    master_list[index].status = 1; //DAYS_INFECTED;
    infected.push_back(&master_list[index]);
    num_i++;
    num_s--;
  }
  return 0;
}



// Prints all people in the world to the terminal
int World::print_people(person* list) {
  int i;
  char buffer[PERSON_BUFFER];

  for(i = 0; i < master_count; i++) {
    printf("%d\n", i);
		print_person(buffer, &list[i]);
		printf("%s\n", buffer);
	}
	
	return 0;
}
	
// Creates a person with a random location between 0 and 1
int World::populate_person_rand(person * p) {
	p->status = 0;
	p->x = rand()/((double)RAND_MAX + 1);
	p->y = rand()/((double)RAND_MAX + 1);
	return 0;
}

// Creates a person on a grid
int World::populate_person_grid(person * p, int index, int rows, int cols) {
	p->status = 0;
  double wid = 1.0 / (rows);
	p->x = (index % cols) * wid + wid/2;
	p->y = (index/cols) * wid + wid/2;
	return 0;
}

// Takes a person and generates a string representing that person
int World::print_person(char *str, person *p) {
	sprintf(str, "%d, (%f, %f)", p->status, p->x, p->y);
	return 0;
}

// This function takes an array of sorted "lowest values", where the first number is highest
// and tries to replace the highest one if new_val is lower than it. also maintains sorting
void try_replace(double *vals, int *indices, int size, double new_val, int new_i) {
  if(new_val >= vals[0])
    return;
  for(int k = size-1; k >= 0; k--) {
    if(new_val < vals[k]) {
      //percolate values up
      for(int j = 0; j < k; j++) {
        vals[j] = vals[j+1];
        indices[j] = indices[j+1];
      }
      //replace lowest applicable value
      vals[k] = new_val;
      indices[k] = new_i;
      return;
    }
  }
}

// Progress the world one step, propegating infection status when appropriate
bool World::step() {
  //progress simulation time by 1
  simtime++;

  printf("simtime: %d\n", simtime);
  //int num_connections = 8; // this is hardcoded, requires changing the below arrays
  int start_size = infected.size();
  for(int i = 0; i < start_size; i++) {
    person p1 = *infected[i];
    //printf("x, y: %f, %f\n\n\n", p1.x, p1.y);
    //double lowest_vals[] = {INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX,INT_MAX};
    //int lowest_indices[] = {-1,-1,-1,-1,-1,-1,-1,-1};
    int p2_index = 0;
    while(true) {
      double r = exp_dist(0);
      double theta = 2 * M_PI * rand()/((double)RAND_MAX+1);
      double x = p1.x + r * cos(theta);
      double y = p1.y + r * sin(theta);
      if(x > 1 || x < 0 || y > 1 || y < 0)
        continue;
      //convert these coordinates to an index:
      int grid_wid = sqrt(master_count);
      double wid = 1.0 / grid_wid;
      // round x, y to nearest
      int col = x * grid_wid;
      int row = y * grid_wid;
      // we've selected the same point as p1
      if(col * wid + wid/2 == p1.x && row * wid + wid/2 == p1.y)
        continue;
      p2_index = row * grid_wid + col;
      break;
    }
    double kappa = .9;
    if((rand()/((double)RAND_MAX + 1)) < kappa)
      infect(p2_index);
  }
  for(int i = 0; i < master_count; i++) {
    if(master_list[i].status > 0) {
      master_list[i].status++;
    }
  }
  return (num_i != 0);
}



// Returns distance between two points on a plane, where the edges of the plane
// are assumed to be "wrapped" to the other side.
double World::toroidal_distance(double x1, double x2, double y1, double y2) {
  double dx = x1 - x2;
  double dy = y1 - y2;
  /*
  if(dx < 0)
    dx = dx * -1.0;
  if(dy < 0)
    dy = dy * -1.0;

  if(dx > .5)
    dx = 1.0 - dx;
  if(dy > .5)
    dy = 1.0 - dy;
  */

  // toroidal distance between people (max = sqrt(2)/2)
  return sqrt(dx*dx + dy*dy);
}
