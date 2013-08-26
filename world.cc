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
  master_matrix = (unsigned long**) malloc(master_count * sizeof(long*));
  for (int i = 0; i < master_count; i++)  
   master_matrix[i] = (unsigned long*) malloc(master_count*sizeof(long));  
  
  populate_matrix(master_matrix, master_list, master_count, &World::exp_dist);
  //we need to generate pairwise probabilities. Assuming this person gets the disease, 
  //what is the expected number of days to pass it to one of his neighbors, based on the contact distribution
  //ceil(log(1-x)/log(1-p))
  simtime = 0;

  //make a priority queue so that the lowest number is first. this will contain infection times for everybody
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


int World::populate_matrix(unsigned long ** matrix, person *people, int n, float (World::*dist_fn)(float)) {
  int i;
  int j;
  for(i = 0; i<n; i++) {
    for(j = 0; j<n; j++) {
      if(i == j)
        matrix[i][j] = 0;
      else {
        matrix[i][j] = exp_contact(&people[i], &people[j], dist_fn);
      }
    }
  }
  return 0;
}

unsigned long World::exp_contact(person *p1, person *p2, float (World::*dist_fn)(float)) {
  float d = toroidal_distance(p1->x, p2->x, p1->y, p2->y);
  float p = (*this.*dist_fn)(d);
  if(p<.0000001)
    return 1000000000; //we will never run for more than a billion steps
  float u = rand()/((float)RAND_MAX + 1);
  return ceil(log(1-u)/log(1-p));
}

float World::linear_dist(float d) {
  return d;
}

float World::exp_dist(float d) {
  float u = rand()/((float)RAND_MAX + 1);

  // exponential random variable
  int a = 1;
  int b = 2;
  int c = 3;
  float r = d * 100;
  return pow(a+r, -b) * exp(-r / c);
  //return exp(-EXP_PARAMETER * d);
}

float World::pow_dist(float d) {
  // uniform random variable in (0, 1)
  float u = rand()/((float)RAND_MAX + 1);

  // power law random variable
  int a = 1;
  int b = 2;
  int c = 300;
  float r = d * 50;
  return pow(a+r, -b) * exp(-r / c);
  //return pow(1 + pow(d*POW_R_0, POW_ALPHA), -1);
}

	
// Creates a list of n people for the world, currently uniformly
// randomly distributed on a (0, 1) torus.
int World::generate_people(person **all_people, int n) {
  //int count = populate_people(*all_people, n);
  printf("generating people\n");
	int i;
	for(i = 0; i < n; i++) {
    printf("person %d\n", i);
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

    unsigned long infect_time;
    //go through and add all neighbors to the priority queue
    for(int j = 0; j < master_count; j++) {
      infect_time = simtime + master_matrix[index][j];
      //don't add people to the queue if they will be infected after
      //we are done simulating
      if(infect_time < MAX_SIM_TIME)
        master_queue.push(std::make_pair(j, infect_time));
    }
    num_i++;
    num_s--;
  }
  return 0;
}

// Cure the person, but provide the index in i_list
// We will have problems if people start to not recover in
// monotonic increasing order
int World::cure(int index) {
  int first_infected = num_r;
  int first_susceptible = num_r + num_i;
  assert(index >= first_infected && index < first_susceptible);

  // Switch places to maintain list order, if necessary.
  if(first_infected != index) {
    person temp = master_list[first_infected];
    master_list[first_infected] = master_list[index];
    master_list[index] = temp;
  }

  master_list[first_infected].status = RECOVERED;
  num_r++;
  num_i--;
  return 0;
}

int World::progress_sickness(int index) {

  //people don't get better, so we are under the SI model
  master_list[index].status++;
  //if(master_list[index].status == SUSCEPTIBLE)
  //  cure(index);
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
	p->x = rand()/((float)RAND_MAX + 1);
	p->y = rand()/((float)RAND_MAX + 1);
	return 0;
}

// Creates a person on a grid
int World::populate_person_grid(person * p, int index, int rows, int cols) {
	p->status = 0;
  double wid = 1.0 / (rows+1);
	p->x = (index % cols) * wid + wid;
	p->y = (index/cols) * wid + wid;
	return 0;
}

int World::populate_people(person *people, int n) {
  ifstream file("data/data31.txt", ifstream::in);
  //these are hardcoded for now, maybe they should be read from the file in the future
  int xdim = 201;
  int ydim = 201;
  char value[30];
  int i;
  int j = 0;
	for(i = 0; i < n; i++) {
    people[j].status = 0;
    file.getline(value, 30, ' ');
    people[j].x = atof(value);
    file.getline(value, 30, '\n');
    people[j].y = atof(value);
    //By incrementing j every other time we halve the data size
    if(0 == i%1) {
      j++;
    }
  }
  printf("%d\n", j);
  return j;
}

// Takes a person and generates a string representing that person
int World::print_person(char *str, person *p) {
	sprintf(str, "%d, (%f, %f)", p->status, p->x, p->y);
	return 0;
}

// Progress the world one step, propegating infection status when appropriate
bool World::step() {
  //progress simulation time by 1
  simtime++;

  printf("simtime: %d\n", simtime);

  while(master_queue.top().second <= simtime) {
    infect(master_queue.top().first);
    master_queue.pop();
  }

  int i = 0;
  for (i = 0; i < master_count; i++) {
    if(master_list[i].status > 0) {
      master_list[i].status++;
    }
  }
  return (num_i != 0);
}



// Randomly simulate contact between people. People are more likely to
// come into contact if they are closer to each other.
bool World::contact_occurs(person a, person b) {
  float d = toroidal_distance(a.x, b.x, a.y, b.y);

  // uniform random variable in (0, 1)
  float u = rand()/((float)RAND_MAX + 1);

  // exponential random variable
  //float e = exp(-EXP_PARAMETER * d);

  // power law random variable
  float p = pow(1 + pow(d*POW_R_0, POW_ALPHA), -1);
  return u < p;
}

// Returns distance between two points on a plane, where the edges of the plane
// are assumed to be "wrapped" to the other side.
float World::toroidal_distance(float x1, float x2, float y1, float y2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
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
