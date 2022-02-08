let tenMinutes = 60. *. 10.
let fiveMinutes = 60. *. 5.
let twoMinutes = 60. *. 2.
let createQuiz ownerId timestamp world =
  let (quizId, quizTitle, questions, world) = World.createQuiz world in
  let timestamp = timestamp +. (Random.float fiveMinutes) in
  let createEvent =
    Events.create ~timestamp
      ~type_:((Events.QuizWasCreated ({ quizId; ownerId; quizTitle }))
      [@explicit_arity ]) in
  let (events, timestamp) =
    List.fold_left
      ~f:(fun (events, timestamp) ->
            fun { World.Quiz.question = question; answer; id } ->
              let timestamp = timestamp +. (Random.float tenMinutes) in
              (((Events.create ~timestamp
                   ~type_:((Events.QuestionAddedToQuiz
                              ({ quizId; questionId = id; question; answer }))
                   [@explicit_arity ])) :: events), timestamp))
      ~init:([createEvent], timestamp) questions in
  let timestamp = timestamp +. (Random.float fiveMinutes) in
  let publishEvent =
    Events.create ~timestamp ~type_:((Events.QuizWasPublished ({ quizId }))
      [@explicit_arity ]) in
  (world, (publishEvent :: events))
let playGame player world timestamp =
  let quiz = World.pickQuiz world in
  match quiz with
  | None -> (world, [])
  | ((Some (quiz))[@explicit_arity ]) ->
      let gameId = Uuid.generateId () in
      let timestamp = timestamp +. (Random.float fiveMinutes) in
      let events =
        [Events.create ~timestamp
           ~type_:((Events.GameWasOpened
                      ({
                         quizId = (quiz.World.Quiz.id);
                         gameId;
                         playerId = (player.World.Player.id)
                       }))[@explicit_arity ])] in
      let playersJoining = World.playersThatJoinAGame timestamp world in
      let timestampPlusFiveMinutes = timestamp +. fiveMinutes in
      let events =
        match playersJoining with
        | [] ->
            (Events.create ~timestamp:timestampPlusFiveMinutes
               ~type_:((Events.GameWasCancelled ({ gameId }))
               [@explicit_arity ]))
            :: events
        | players ->
            let events =
              List.fold_left
                ~f:(fun events ->
                      fun player ->
                        (Events.create
                           ~timestamp:(timestamp +.
                                         (Random.float fiveMinutes))
                           ~type_:((Events.PlayerJoinedGame
                                      ({
                                         playerId = (player.World.Player.id);
                                         gameId
                                       }))[@explicit_arity ]))
                        :: events) ~init:events players in
            let events =
              (Events.create ~timestamp:timestampPlusFiveMinutes
                 ~type_:((Events.GameWasStarted ({ gameId }))
                 [@explicit_arity ]))
              :: events in
            let timestamp = timestampPlusFiveMinutes in
            let (events, timestamp) =
              List.fold_left
                ~f:(fun (events, timestamp) ->
                      fun question ->
                        let questionEvent =
                          Events.create ~timestamp
                            ~type_:((Events.QuestionWasAsked
                                       ({
                                          gameId;
                                          questionId =
                                            (question.World.Quiz.id)
                                        }))[@explicit_arity ]) in
                        let responseEvents =
                          List.map
                            ~f:(fun player ->
                                  let answerType =
                                    World.Player.answerQuestion player in
                                  let timestamp =
                                    match answerType with
                                    | `AnswerCorrectly speed ->
                                        timestamp +. speed
                                    | `AnswerIncorrectly speed ->
                                        timestamp +. speed
                                    | `AnswerTimeout ->
                                        timestamp +. twoMinutes in
                                  let type_ =
                                    match answerType with
                                    | `AnswerCorrectly _ ->
                                        ((Events.AnswerWasGiven
                                            ({
                                               questionId =
                                                 (question.World.Quiz.id);
                                               gameId;
                                               playerId =
                                                 (player.World.Player.id);
                                               answer =
                                                 (question.World.Quiz.answer)
                                             }))
                                        [@explicit_arity ])
                                    | `AnswerIncorrectly _ ->
                                        ((Events.AnswerWasGiven
                                            ({
                                               questionId =
                                                 (question.World.Quiz.id);
                                               gameId;
                                               playerId =
                                                 (player.World.Player.id);
                                               answer =
                                                 "TODO random incorrect answer"
                                             }))
                                        [@explicit_arity ])
                                    | `AnswerTimeout ->
                                        ((Events.TimerHasExpired
                                            ({
                                               questionId =
                                                 (question.World.Quiz.id);
                                               gameId;
                                               playerId =
                                                 (player.World.Player.id)
                                             }))
                                        [@explicit_arity ]) in
                                  Events.create ~timestamp ~type_) players in
                        let timestamp = timestamp +. twoMinutes in
                        (((Events.create ~timestamp
                             ~type_:((Events.QuestionWasCompleted
                                        ({
                                           gameId;
                                           questionId =
                                             (question.World.Quiz.id)
                                         }))[@explicit_arity ])) ::
                          (responseEvents @ (questionEvent :: events))),
                          timestamp)) ~init:(events, timestamp)
                quiz.World.Quiz.questions in
            (Events.create ~timestamp
               ~type_:((Events.GameWasFinished ({ gameId }))
               [@explicit_arity ]))
              :: events in
      (world, events)
let createPlayer timestamp world =
  let timestamp = timestamp +. (Random.float tenMinutes) in
  let (playerId, worldUpdate) = World.createPlayer world in
  (worldUpdate,
    (Events.create ~timestamp
       ~type_:((Events.PlayerHasRegistered
                  ({
                     playerId;
                     firstName = (Faker.firstName ());
                     lastName = (Faker.lastName ())
                   }))[@explicit_arity ])))
let createPlayerHandler timestamp world =
  if World.shouldCreatePlayer timestamp world
  then
    let (world, playerHasRegistered) = createPlayer timestamp world in
    (world, [playerHasRegistered])
  else (world, [])
let createQuizHandler timestamp world =
  let playersCreatingQuiz = World.playersCreatingQuiz timestamp world in
  List.fold_left ~init:(world, [])
    ~f:(fun (world, events) ->
          fun player ->
            let (world, newEvents) =
              createQuiz player.World.Player.id timestamp world in
            (world, (newEvents @ events))) playersCreatingQuiz
let playGameHandler timestamp world =
  let playersOpeningGame = World.playersOpeningGame timestamp world in
  List.fold_left ~init:(world, [])
    ~f:(fun (world, events) ->
          fun player ->
            let (world, newEvents) = playGame player world timestamp in
            (world, (newEvents @ events))) playersOpeningGame
let handlers = [createPlayerHandler; createQuizHandler; playGameHandler]
let handleTick timestamp world =
  List.fold_left ~init:(world, [])
    ~f:(fun (world, events) ->
          fun handler ->
            let (world, newEvents) = handler timestamp world in
            (world, (newEvents @ events))) handlers
type t = {
  timeRange: TimeRange.t ;
  world: World.t }
let create ~timeRange  ~playerDistribution  ~createPlayerDistribution  =
  let world = World.init ~playerDistribution ~createPlayerDistribution in
  { timeRange; world }
let run t =
  let rec run' timestamp endTimestamp events world =
    if (Float.compare timestamp endTimestamp) <= 0
    then
      let (world, newEvents) = handleTick timestamp world in
      run' (timestamp +. tenMinutes) endTimestamp (newEvents @ events) world
    else List.rev events in
  run' (t.timeRange).TimeRange.startTimestamp
    (t.timeRange).TimeRange.endTimestamp [] t.world