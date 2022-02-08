let memory = Bytes.of_string "2000-00-00T00:00:00Z"
let set pos int = Bytes.unsafe_set memory pos (Char.of_int_exn (48 + int))
let format
  ({ Unix.tm_year = tm_year; tm_mon; tm_mday; tm_hour; tm_min; tm_sec;_} :
    Unix.tm)
  =
  set 2 ((tm_year / 10) % 10);
  set 3 (tm_year % 10);
  set 5 (((tm_mon + 1) / 10) % 10);
  set 6 ((tm_mon + 1) % 10);
  set 8 ((tm_mday / 10) % 10);
  set 9 (tm_mday % 10);
  set 11 ((tm_hour / 10) % 10);
  set 12 (tm_hour % 10);
  set 14 ((tm_min / 10) % 10);
  set 15 (tm_min % 10);
  set 17 ((tm_sec / 10) % 10);
  set 18 (tm_sec % 10);
  Bytes.to_string memory
type event = {
  id: Uuid.t ;
  timestamp: float ;
  type_: payload }[@@deriving show]
and payload =
  | PlayerHasRegistered of
  {
  playerId: Uuid.t ;
  lastName: string ;
  firstName: string } 
  | QuizWasCreated of {
  quizId: Uuid.t ;
  ownerId: Uuid.t ;
  quizTitle: string } 
  | QuestionAddedToQuiz of
  {
  quizId: Uuid.t ;
  questionId: Uuid.t ;
  question: string ;
  answer: string } 
  | QuizWasPublished of {
  quizId: Uuid.t } 
  | GameWasOpened of {
  quizId: Uuid.t ;
  gameId: Uuid.t ;
  playerId: Uuid.t } 
  | GameWasCancelled of {
  gameId: Uuid.t } 
  | GameWasStarted of {
  gameId: Uuid.t } 
  | PlayerJoinedGame of {
  playerId: Uuid.t ;
  gameId: Uuid.t } 
  | QuestionWasAsked of {
  gameId: Uuid.t ;
  questionId: Uuid.t } 
  | TimerHasExpired of
  {
  questionId: Uuid.t ;
  playerId: Uuid.t ;
  gameId: Uuid.t } 
  | GameWasFinished of {
  gameId: Uuid.t } 
  | AnswerWasGiven of
  {
  questionId: Uuid.t ;
  playerId: Uuid.t ;
  gameId: Uuid.t ;
  answer: string } 
  | QuestionWasCompleted of {
  questionId: Uuid.t ;
  gameId: Uuid.t } 
let create ~timestamp  ~type_  =
  { id = (Uuid.generateId ()); type_; timestamp }
let toTimestamp t =
  (CalendarLib.Calendar.Precise.from_unixfloat t) |>
    (CalendarLib.Printer.Precise_Calendar.sprint "%iT%T%z")
let toJson event =
  let stringType =
    match event.type_ with
    | AnswerWasGiven _ -> "AnswerWasGiven"
    | PlayerJoinedGame _ -> "PlayerJoinedGame"
    | PlayerHasRegistered _ -> "PlayerHasRegistered"
    | QuizWasCreated _ -> "QuizWasCreated"
    | QuestionAddedToQuiz _ -> "QuestionAddedToQuiz"
    | QuestionWasAsked _ -> "QuestionWasAsked"
    | QuestionWasCompleted _ -> "QuestionWasCompleted"
    | QuizWasPublished _ -> "QuizWasPublished"
    | GameWasOpened _ -> "GameWasOpened"
    | GameWasCancelled _ -> "GameWasCancelled"
    | GameWasStarted _ -> "GameWasStarted"
    | GameWasFinished _ -> "GameWasFinished"
    | TimerHasExpired _ -> "TimerHasExpired" in
  let payload =
    match event.type_ with
    | ((AnswerWasGiven
        ({ gameId; questionId; playerId; answer }))[@explicit_arity ]) ->
        `Assoc
          [("game_id", (`String (gameId |> Uuid.to_string)));
          ("question_id", (`String (questionId |> Uuid.to_string)));
          ("player_id", (`String (playerId |> Uuid.to_string)));
          ("answer", (`String answer))]
    | ((PlayerJoinedGame ({ gameId; playerId }))[@explicit_arity ]) ->
        `Assoc
          [("game_id", (`String (gameId |> Uuid.to_string)));
          ("player_id", (`String (playerId |> Uuid.to_string)))]
    | ((PlayerHasRegistered
        ({ playerId; lastName; firstName }))[@explicit_arity ]) ->
        `Assoc
          [("player_id", (`String (playerId |> Uuid.to_string)));
          ("last_name", (`String lastName));
          ("first_name", (`String firstName))]
    | ((QuizWasCreated ({ quizTitle; quizId; ownerId }))[@explicit_arity ])
        ->
        `Assoc
          [("quiz_title", (`String quizTitle));
          ("quiz_id", (`String (quizId |> Uuid.to_string)));
          ("owner_id", (`String (ownerId |> Uuid.to_string)))]
    | ((QuestionAddedToQuiz
        ({ quizId; questionId; question; answer }))[@explicit_arity ]) ->
        `Assoc
          [("quiz_id", (`String (quizId |> Uuid.to_string)));
          ("question_id", (`String (questionId |> Uuid.to_string)));
          ("question", (`String question));
          ("answer", (`String answer))]
    | ((QuestionWasAsked ({ gameId; questionId }))[@explicit_arity ]) ->
        `Assoc
          [("game_id", (`String (gameId |> Uuid.to_string)));
          ("question_id", (`String (questionId |> Uuid.to_string)))]
    | ((QuestionWasCompleted ({ gameId; questionId }))[@explicit_arity ]) ->
        `Assoc
          [("game_id", (`String (gameId |> Uuid.to_string)));
          ("question_id", (`String (questionId |> Uuid.to_string)))]
    | ((QuizWasPublished ({ quizId }))[@explicit_arity ]) ->
        `Assoc [("quiz_id", (`String (quizId |> Uuid.to_string)))]
    | ((GameWasOpened ({ quizId; gameId; playerId }))[@explicit_arity ]) ->
        `Assoc
          [("quiz_id", (`String (quizId |> Uuid.to_string)));
          ("game_id", (`String (gameId |> Uuid.to_string)));
          ("player_id", (`String (playerId |> Uuid.to_string)))]
    | ((GameWasCancelled ({ gameId }))[@explicit_arity ]) ->
        `Assoc [("game_id", (`String (gameId |> Uuid.to_string)))]
    | ((GameWasStarted ({ gameId }))[@explicit_arity ]) ->
        `Assoc [("game_id", (`String (gameId |> Uuid.to_string)))]
    | ((GameWasFinished ({ gameId }))[@explicit_arity ]) ->
        `Assoc [("game_id", (`String (gameId |> Uuid.to_string)))]
    | ((TimerHasExpired
        ({ gameId; questionId; playerId }))[@explicit_arity ]) ->
        `Assoc
          [("game_id", (`String (gameId |> Uuid.to_string)));
          ("question_id", (`String (questionId |> Uuid.to_string)));
          ("player_id", (`String (playerId |> Uuid.to_string)))] in
  let timestamp = let open Unix in format (gmtime event.timestamp) in
  `Assoc
    [("id", (`String (Uuid.to_string event.id)));
    ("type", (`String stringType));
    ("timestamp", (`String timestamp));
    ("payload", payload)]