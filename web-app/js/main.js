// UI elements.
const deviceNameLabel = document.getElementById('device-name');
const connectButton = document.getElementById('connect');
const disconnectButton = document.getElementById('disconnect');
const terminalContainer = document.getElementById('terminal');
const sendForm = document.getElementById('send-form');
const inputField = document.getElementById('input');
const inputButton = document.getElementById('input-button');

// Helpers.
const defaultDeviceName = 'Terminal';
const terminalAutoScrollingLimit = terminalContainer.offsetHeight / 2;
let isTerminalAutoScrolling = true;

const scrollElement = (element) => {
  const scrollTop = element.scrollHeight - element.offsetHeight;

  if (scrollTop > 0) {
    element.scrollTop = scrollTop;
  }
};

const logToTerminal = (message, type = '') => {
  terminalContainer.insertAdjacentHTML('beforeend',
    `<div${type && ` class="${type}"`}>${message}</div>`);

  if (isTerminalAutoScrolling) {
    scrollElement(terminalContainer);
  }
};

// Obtain configured instance.
const terminal = new BluetoothTerminal();

// Override `receive` method to log incoming data to the terminal.
terminal.receive = function (data) {
  logToTerminal(data, 'in');
  console.log(data)
  if (data == "Incorrect password") {
    inputButton.classList.add('btn-danger');
    inputButton.classList.remove('btn-primary');
    inputButton.value = 'Incorrect Password';


    setTimeout(function () {
      inputButton.classList.add('btn-primary');
      inputButton.classList.remove('btn-danger');
      inputButton.value = 'Unlock';
    }, 2000);

  } else if (data == "Unlocked") {
    inputButton.classList.add('btn-success');
    inputButton.classList.remove('btn-primary');
    inputButton.value = 'Unlocked';

  } else if (data == "Generated new psw") {
    inputButton.classList.add('btn-primary');
    inputButton.classList.remove('btn-success');
    inputButton.value = 'Unlock';
  }
};

// Override default log method to output messages to the terminal and console.
terminal._log = function (...messages) {
  // We can't use `super._log()` here.
  messages.forEach((message) => {
    logToTerminal(message);
    console.log(message); // eslint-disable-line no-console
  });
};

// Implement own send function to log outcoming data to the terminal.
const send = (data) => {
  terminal.send(data).
    then(() => logToTerminal(data, 'out')).
    catch((error) => logToTerminal(error));
};

// Bind event listeners to the UI elements.
connectButton.addEventListener('click', () => {
  terminal.connect().
    then(() => {
      // deviceNameLabel.textContent = terminal.getDeviceName() ?
      //     terminal.getDeviceName() : defaultDeviceName;
    });
});

// disconnectButton.addEventListener('click', () => {
//   terminal.disconnect();
//   deviceNameLabel.textContent = defaultDeviceName;
// });

sendForm.addEventListener('submit', (event) => {
  event.preventDefault();

  send(inputField.value);

  inputField.value = '';
  inputField.focus();
});

// Switch terminal auto scrolling if it scrolls out of bottom.
terminalContainer.addEventListener('scroll', () => {
  const scrollTopOffset = terminalContainer.scrollHeight -
    terminalContainer.offsetHeight - terminalAutoScrollingLimit;

  isTerminalAutoScrolling = (scrollTopOffset < terminalContainer.scrollTop);
});
