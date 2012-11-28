#define PERSON_BUFFER 100
#define NUM_PEOPLE    5000
#define DAYS_INFECTED 10
#define EXP_PARAMETER 120.0
#define SUSCEPTIBLE   0
#define RECOVERED     -1
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
  int print_people(person[]);
  int step();

private:
  person* generate_people(int);
  int populate_person(person *);
  int print_person(char *, person *);
  bool contact_occurs(person, person);
  float toroidal_distance(float, float, float, float);
  int infect(int);
  int cure(int);
  int progress_sickness(int);
  void delete_me();

  person* master_list;
  // Numbers of people infected, susceptible, or recovered
  int num_i;
  int num_s;
  int num_r;
};
