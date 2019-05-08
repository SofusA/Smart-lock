import React, { Component } from 'react';
import axios from 'axios';
import logo from './logo.svg';
import './App.css';

class App extends Component {

  constructor () {
    super()
    this.state = {
      newKey: 42
    }
    this.handleClick = this.handleClick.bind(this)
  }

  handleClick () {
    var config = {
      headers: {'Access-Control-Allow-Origin': '*'}
    };
    axios.get('http://localhost:3000/api/getKey', config)
        .then(response => this.setState({newKey: response.data}))
    this.setState({newKey: this.state.newKey+1})

  }

  render() {
    return (
      <div className="App">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />

          <button className='button' onClick={this.handleClick}>
            Get Key to Smart Lock
          </button>
          <p>{this.state.newKey}</p>

        </header>
      </div>
    );
  }
}

export default App;
