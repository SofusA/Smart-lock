const inputButton = document.getElementById('input-button');
const sendForm = document.getElementById('send-form');
const unlockUrl = '../pass';

const inputField = document.getElementById('input');

inputButton.addEventListener('click', () => {
    
});

sendForm.addEventListener('submit', (event) => {
    event.preventDefault();

    fetch(unlockUrl + '?pass=' + inputField.value)
    .then(data=>{return data.json()})
    .then(res=>console.log(res))

    inputField.value = '';
    inputField.focus();
});