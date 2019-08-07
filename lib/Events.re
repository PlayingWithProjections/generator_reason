let memory = Bytes.of_string("2000-00-00T00:00:00Z");
let set = (pos, int) => Bytes.unsafe_set(memory, pos, Char.chr(48 + int));

let format =
    ({Unix.tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, _}: Unix.tm) => {
  set(2, tm_year / 10 mod 10);
  set(3, tm_year mod 10);
  set(5, (tm_mon + 1) / 10 mod 10);
  set(6, (tm_mon + 1) mod 10);
  set(8, tm_mday / 10 mod 10);
  set(9, tm_mday mod 10);
  set(11, tm_hour / 10 mod 10);
  set(12, tm_hour mod 10);
  set(14, tm_min / 10 mod 10);
  set(15, tm_min mod 10);
  set(17, tm_sec / 10 mod 10);
  set(18, tm_sec mod 10);
  Bytes.to_string(memory);
};
[@deriving show]
type event = {
  id: Uuid.t,
  timestamp: float,
  type_: payload,
}
and payload =
  | PlayerHasRegistered({
      playerId: Uuid.t,
      lastName: string,
      firstName: string,
    })
  | QuizWasCreated({
      quizId: Uuid.t,
      ownerId: Uuid.t,
      quizTitle: string,
    })
  | QuestionAddedToQuiz({
      quizId: Uuid.t,
      questionId: Uuid.t,
      question: string,
      answer: string,
    })
  | QuizWasPublished({quizId: Uuid.t})
  | GameWasOpened({
      quizId: Uuid.t,
      gameId: Uuid.t,
      playerId: Uuid.t,
    })
  | GameWasCancelled({gameId: Uuid.t})
  | GameWasStarted({gameId: Uuid.t})
  | PlayerJoinedGame({
      playerId: Uuid.t,
      gameId: Uuid.t,
    })
  | QuestionWasAsked({
      gameId: Uuid.t,
      questionId: Uuid.t,
    })
  | TimerHasExpired({
      questionId: Uuid.t,
      playerId: Uuid.t,
      gameId: Uuid.t,
    })
  | GameWasFinished({gameId: Uuid.t})
  | AnswerWasGiven({
      questionId: Uuid.t,
      playerId: Uuid.t,
      gameId: Uuid.t,
      answer: string,
    })
  | QuestionWasCompleted({
      questionId: Uuid.t,
      gameId: Uuid.t,
    });

let create = (~timestamp, ~type_) => {
  id: Uuid.generateId(),
  type_,
  timestamp,
};

let toTimestamp = t =>
  CalendarLib.Calendar.Precise.from_unixfloat(t)
  |> CalendarLib.Printer.Precise_Calendar.sprint("%iT%T%z");

let toJson = event => {
  let stringType =
    switch (event.type_) {
    | AnswerWasGiven(_) => "AnswerWasGiven"
    | PlayerJoinedGame(_) => "PlayerJoinedGame"
    | PlayerHasRegistered(_) => "PlayerHasRegistered"
    | QuizWasCreated(_) => "QuizWasCreated"
    | QuestionAddedToQuiz(_) => "QuestionAddedToQuiz"
    | QuestionWasAsked(_) => "QuestionWasAsked"
    | QuestionWasCompleted(_) => "QuestionWasCompleted"
    | QuizWasPublished(_) => "QuizWasPublished"
    | GameWasOpened(_) => "GameWasOpened"
    | GameWasCancelled(_) => "GameWasCancelled"
    | GameWasStarted(_) => "GameWasStarted"
    | GameWasFinished(_) => "GameWasFinished"
    | TimerHasExpired(_) => "TimerHasExpired"
    };
  let payload =
    switch (event.type_) {
    | AnswerWasGiven({gameId, questionId, playerId, answer}) =>
      `Assoc([
        ("game_id", `String(gameId |> Uuid.to_string)),
        ("question_id", `String(questionId |> Uuid.to_string)),
        ("player_id", `String(playerId |> Uuid.to_string)),
        ("answer", `String(answer)),
      ])
    | PlayerJoinedGame({gameId, playerId}) =>
      `Assoc([
        ("game_id", `String(gameId |> Uuid.to_string)),
        ("player_id", `String(playerId |> Uuid.to_string)),
      ])
    | PlayerHasRegistered({playerId, lastName, firstName}) =>
      `Assoc([
        ("player_id", `String(playerId |> Uuid.to_string)),
        ("last_name", `String(lastName)),
        ("first_name", `String(firstName)),
      ])
    | QuizWasCreated({quizTitle, quizId, ownerId}) =>
      `Assoc([
        ("quiz_title", `String(quizTitle)),
        ("quiz_id", `String(quizId |> Uuid.to_string)),
        ("owner_id", `String(ownerId |> Uuid.to_string)),
      ])
    | QuestionAddedToQuiz({quizId, questionId, question, answer}) =>
      `Assoc([
        ("quiz_id", `String(quizId |> Uuid.to_string)),
        ("question_id", `String(questionId |> Uuid.to_string)),
        ("question", `String(question)),
        ("answer", `String(answer)),
      ])
    | QuestionWasAsked({gameId, questionId}) =>
      `Assoc([
        ("game_id", `String(gameId |> Uuid.to_string)),
        ("question_id", `String(questionId |> Uuid.to_string)),
      ])
    | QuestionWasCompleted({gameId, questionId}) =>
      `Assoc([
        ("game_id", `String(gameId |> Uuid.to_string)),
        ("question_id", `String(questionId |> Uuid.to_string)),
      ])
    | QuizWasPublished({quizId}) =>
      `Assoc([("quiz_id", `String(quizId |> Uuid.to_string))])
    | GameWasOpened({quizId, gameId, playerId}) =>
      `Assoc([
        ("quiz_id", `String(quizId |> Uuid.to_string)),
        ("game_id", `String(gameId |> Uuid.to_string)),
        ("player_id", `String(playerId |> Uuid.to_string)),
      ])
    | GameWasCancelled({gameId}) =>
      `Assoc([("game_id", `String(gameId |> Uuid.to_string))])
    | GameWasStarted({gameId}) =>
      `Assoc([("game_id", `String(gameId |> Uuid.to_string))])
    | GameWasFinished({gameId}) =>
      `Assoc([("game_id", `String(gameId |> Uuid.to_string))])
    | TimerHasExpired({gameId, questionId, playerId}) =>
      `Assoc([
        ("game_id", `String(gameId |> Uuid.to_string)),
        ("question_id", `String(questionId |> Uuid.to_string)),
        ("player_id", `String(playerId |> Uuid.to_string)),
      ])
    };
  let timestamp = {
    Unix.(format(gmtime(event.timestamp)));
  };

  /* let timestamp = */
  /*   Core.Option.value_exn(Ptime.of_float_s(float_of_int(event.timestamp))) */
  /*   |> Ptime.to_rfc3339(~tz_offset_s=0); */
  `Assoc([
    ("id", `String(Uuid.to_string(event.id))),
    ("type", `String(stringType)),
    ("timestamp", `String(timestamp)),
    ("payload", payload),
  ]);
};
