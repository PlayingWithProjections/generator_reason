module Quiz =
  struct
    type quizType =
      | Normal 
      | Boring 
      | Popular 
    type question = {
      question: string ;
      answer: string ;
      id: Uuid.t }
    type t = {
      id: Uuid.t ;
      quizType: quizType ;
      questions: question list }
    let create ~id  ~questions  = { id; questions; quizType = Normal }
  end
module Player =
  struct
    type t =
      {
      id: Uuid.t ;
      answerType: PlayerType.answerType ;
      createQuizDistribution: Distribution.MonthDistribution.t ;
      joinGameDistribution: Distribution.MonthDistribution.t ;
      openGameDistribution: Distribution.MonthDistribution.t }
    let create ~id  ~answerType  ~createQuizDistribution 
      ~joinGameDistribution  ~openGameDistribution  =
      {
        id;
        createQuizDistribution;
        joinGameDistribution;
        openGameDistribution;
        answerType
      }
    let answerQuestion player =
      let maximumTime = 120. in
      let speed =
        Distribution.gaussianCapped
          ~mu:((player.answerType).delay *. maximumTime)
          ~sigma:((player.answerType).delayRange *. maximumTime)
          ~lowerBound:0. ~upperBound:maximumTime in
      if (Float.compare maximumTime speed) = 0
      then `AnswerTimeout
      else
        (let correct =
           (Float.compare (Random.float (player.answerType).correctness) 0.5)
             > 0 in
         if correct then `AnswerCorrectly speed else `AnswerIncorrectly speed)
    let shouldCreateQuiz timestamp player =
      Distribution.MonthDistribution.happens timestamp
        player.createQuizDistribution
    let shouldOpenGame timestamp player =
      Distribution.MonthDistribution.happens timestamp
        player.openGameDistribution
    let shouldJoinGame timestamp player =
      Distribution.MonthDistribution.happens timestamp
        player.joinGameDistribution
  end
module Game =
  struct
    type t = {
      quiz: Quiz.t ;
      id: Uuid.t ;
      players: Player.t list }
    let create ~quiz  ~id  = { id; quiz; players = [] }
    let join ~game  ~player  =
      { game with players = (player :: (game.players)) }
  end
type t =
  {
  playerDistribution: PlayerType.t Distribution.PercentageDistribution.t ;
  createPlayerDistribution: Distribution.MonthDistribution.t ;
  players: Player.t list ;
  quizzes: Quiz.t list }
let init ~playerDistribution  ~createPlayerDistribution  =
  { playerDistribution; createPlayerDistribution; players = []; quizzes = []
  }
let createPlayerWithType ~world  ~playerType  =
  let id = Uuid.generateId () in
  let player =
    Player.create ~id ~answerType:(playerType.PlayerType.answerType)
      ~createQuizDistribution:(playerType.createQuizDistribution)
      ~joinGameDistribution:(playerType.joinGameDistribution)
      ~openGameDistribution:(playerType.openGameDistribution) in
  (id, { world with players = (player :: (world.players)) })
let createPlayer world =
  let playerType =
    Distribution.PercentageDistribution.pick world.playerDistribution in
  createPlayerWithType ~world ~playerType
let createQuiz world =
  let generateRandomAmountOfQuestions () =
    let nbOfQuestions = Random.int 10 in
    List.map
      ~f:(fun (question, answer) ->
            { Quiz.question = question; answer; id = (Uuid.generateId ()) })
      (Faker.quiz nbOfQuestions) in
  let id = Uuid.generateId () in
  let questions = generateRandomAmountOfQuestions () in
  let quiz = Quiz.create ~id ~questions in
  let quizTitle = Faker.quizName () in
  (id, quizTitle, questions,
    { world with quizzes = (quiz :: (world.quizzes)) })
let playersOpeningGame timestamp world =
  List.filter_map world.players
    ~f:(fun player ->
          if Player.shouldOpenGame timestamp player
          then ((Some (player))[@explicit_arity ])
          else None)
let shouldCreatePlayer timestamp world =
  Distribution.MonthDistribution.happens timestamp
    world.createPlayerDistribution
let playersThatJoinAGame timestamp world =
  List.filter_map world.players
    ~f:(fun player ->
          match Player.shouldJoinGame timestamp player with
          | true -> ((Some (player))[@explicit_arity ])
          | false -> None)
let playersCreatingQuiz timestamp world =
  List.filter_map world.players
    ~f:(fun player ->
          if Player.shouldCreateQuiz timestamp player
          then ((Some (player))[@explicit_arity ])
          else None)
let pickQuiz world = Distribution.randomFromList world.quizzes