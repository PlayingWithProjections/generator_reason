let firstNameLength = Array.length(FakerData.firstNames);
let lastNameLength = Array.length(FakerData.lastNames);

let firstName = () => FakerData.firstNames[Random.int(firstNameLength)];
let lastName = () => FakerData.lastNames[Random.int(lastNameLength)];
