#define PERSON_BUFFER 100
#define NUM_PEOPLE    10000
#define DAYS_INFECTED 10
//#define EXP_PARAMETER 53.0
#define EXP_PARAMETER 70.0
#define POW_ALPHA     4
//#define POW_R_0       50.0
#define POW_R_0       70.0
#define SUSCEPTIBLE   0
#define RECOVERED     -1
#define MAX_SIM_TIME  300

#include <queue>

class World
{
public:
  typedef struct person {
    // 0 = Susceptible
    // 1 - 254 = Infected
    // 255 = Dead/safe/recovered
    char status;
    float x;
    float y;
  } person;

  World();
  virtual ~World();

  person* get_people();
  int get_count();
  int print_people(person[]);
  bool step();

private:
  int generate_people(person **, int);
  int populate_person(person *);
  int populate_people(person *, int);
  int print_person(char *, person *);
  bool contact_occurs(person, person);
  float toroidal_distance(float, float, float, float);
  int infect(int);
  int cure(int);
  int progress_sickness(int);
  int populate_matrix(unsigned long **, person *, int, float (World::*)(float));
  unsigned long exp_contact(person *, person *, float (World::*)(float));
  float linear_dist(float);
  float exp_dist(float);
  float pow_dist(float);

  //A list of all people that exist.
  person* master_list;
  //The total number of people
  int master_count;
  //A 2d-array listing how long each person would take to infect the others
  unsigned long **master_matrix;
  // Numbers of people infected, susceptible, or recovered
  int num_i;
  int num_s;
  int num_r;

  //keeps track of the current simulation time, in steps
  int simtime;
  /*
     template< typename FirstType, typename SecondType >
     struct PairComparator {
     bool operator()( const std::pair<FirstType, SecondType>& p1,
     const std::pair<FirstType, SecondType>& p2 ) const 
     {    
     return p1.second > p2.second;
     }
     }
     */
  struct sort_pred {
    bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) {
      return left.second > right.second;
    }
  };  

  typedef std::priority_queue< std::pair<int, int>, std::vector<std::pair<int,int> >, sort_pred > personPQ;

  personPQ master_queue;
};
