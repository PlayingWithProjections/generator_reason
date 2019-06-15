[@deriving show]
type event = {
  id: Uuid.t,
  timestamp: int,
  type_: payload,
}
and payload =
  | PlayerHasRegistered{
      playerId: Uuid.t,
      lastName: string,
      firstName: string,
    }
  | QuizWasCreated{
      quizId: Uuid.t,
      ownerId: Uuid.t,
      quizTitle: string,
    }
  | QuestionAddedToQuiz{
      quizId: Uuid.t,
      questionId: Uuid.t,
      question: string,
      answer: string,
    }
  | QuizWasPublished{quizId: Uuid.t}
  | GameWasOpened{
      quizId: Uuid.t,
      gameId: Uuid.t,
      playerId: Uuid.t,
    }
  | GameWasCancelled{gameId: Uuid.t}
  | GameWasStarted{gameId: Uuid.t}
  | PlayerJoinedGame{
      playerId: Uuid.t,
      gameId: Uuid.t,
    }
  | QuestionWasAsked{
      gameId: Uuid.t,
      questionId: Uuid.t,
    }
  | TimerHasExpired{
      questionId: Uuid.t,
      playerId: Uuid.t,
      gameId: Uuid.t,
    }
  | GameWasFinished{gameId: Uuid.t}
  | AnswerWasGiven{
      questionId: Uuid.t,
      playerId: Uuid.t,
      gameId: Uuid.t,
      answer: string,
    }
  | QuestionWasCompleted{
      questionId: Uuid.t,
      gameId: Uuid.t,
    };

let create = (~timestamp, ~type_) => {
  id: Uuid.generateId(),
  type_,
  timestamp,
};
let toTimestamp = t =>
  CalendarLib.Calendar.Precise.from_unixfloat(float_of_int(t))
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
  	open Core.Unix;
  	strftime(gmtime(float_of_int(event.timestamp)), "%FT%TZ");
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
