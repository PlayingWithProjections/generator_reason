include Uuidm;

let randomState = Random.State.make([|1, 2, 3|]);

let generateId = () => v4_gen(randomState, ());
