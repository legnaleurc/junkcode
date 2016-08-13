import React from 'react';


export class WeaponInputView extends React.Component {

  render () {
    return (
      <dl class="weapon">
        <dd class="attack"><input type="text" onChange={this.onAttackChange.bind(this)} /></dd>
        <dd class="attack"><WeaponTypeInputView onChange={this.onTypeChange.bind(this)} /></dd>
        <dd class="attack"><input type="text" onChange={this.onAttackChange.bind(this)} /></dd>
        <dd class="attack"><input type="text" onChange={this.onAttackChange.bind(this)} /></dd>
        <dd class="attack"><input type="text" onChange={this.onAttackChange.bind(this)} /></dd>
        <dd class="attack"><input type="text" onChange={this.onAttackChange.bind(this)} /></dd>
        <dd class="attack"><input type="text" onChange={this.onAttackChange.bind(this)} /></dd>
      </dl>
    );
  }

  onAttackChange (event) {
  }

  onTypeChange (event) {
  }

}


class WeaponTypeInputView extends React.Component {

  static get TYPES () {
    return {};
  }

  render () {
    return (
      <select class="type">
      </select>
    );
  }

}
