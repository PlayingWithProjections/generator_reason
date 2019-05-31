type frequency =
  | PerDay(int)
  | OneIn(int);

let minutesPerDay = 24 * 60;

let happens = frequency => {
  switch (frequency) {
  | PerDay(times) => Random.int(minutesPerDay / times) == 0
  | OneIn(x) => Random.int(x) == 0
  };
};

let listRandom = l => {
  let length = List.length(l);
  let i = Random.int(length);
  List.nth(l, i);
};
