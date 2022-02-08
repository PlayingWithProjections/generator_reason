let firstNameLength = Array.length FakerData.firstNames
let lastNameLength = Array.length FakerData.lastNames
let firstName () = FakerData.firstNames.(Random.int firstNameLength)
let lastName () = FakerData.lastNames.(Random.int lastNameLength)
let quizzes =
  [|(FakerData.gameOfThronesQuotes, FakerData.gameOfThronesCharacters);
    (FakerData.ghostbusterQuotes, FakerData.ghostbusterCharacters)|]
let quizzesLength = Array.length quizzes
let funnyNameLength = Array.length FakerData.funnyNames
let hackerVerbsLength = Array.length FakerData.hackerVerbs
let quizName () =
  (FakerData.hackerVerbs.(Random.int hackerVerbsLength)) ^
    (" " ^ (FakerData.funnyNames.(Random.int funnyNameLength)))
let quiz nbOfQuestions =
  let arrayPicker arr =
    let arrLength = Array.length arr in fun () -> arr.(Random.int arrLength) in
  let rec pick remaining picked questionPicker answerPicker =
    if remaining = 0
    then picked
    else
      pick (remaining - 1) (((questionPicker ()), (answerPicker ())) ::
        picked) questionPicker answerPicker in
  let (questions, answers) = quizzes.(Random.int quizzesLength) in
  pick nbOfQuestions [] (arrayPicker questions) (arrayPicker answers)