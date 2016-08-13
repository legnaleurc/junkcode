const ATTRIBUTES = {
  '1': {
    name: '火',
  },
  '2': {
    name: '水',
  },
  '3': {
    name: '土',
  },
  '4': {
    name: '風',
  },
  '5': {
    name: '光',
  },
  '6': {
    name: '闇',
  },
};


const KINDS = {
  '1': {
    name: '剣',
  },
  '2': {
    name: '短剣',
  },
  '3': {
    name: '槍',
  },
  '4': {
    name: '斧',
  },
  '5': {
    name: '杖',
  },
  '6': {
    name: '銃',
  },
  '7': {
    name: '格闘',
  },
  '8': {
    name: '弓',
  },
  '9': {
    name: '楽器',
  },
  '10': {
    name: '刀',
  },
};


export class Weapon {

  static toJSON (key, value) {
    if (value instanceof Weapon) {
      return value._;
    } else if (value instanceof WeaponSkill) {
      return value._;
    }
    return value;
  }

  constructor (weapon) {
    this._ = {
      id: weapon.id,
      name: weapon.master.name,
      kind: weapon.master.kind,
      attribute: weapon.master.attribute,
      attack: parseInt(weapon.param.attack, 10),
      skillLevel: parseInt(weapon.param.skill_level, 10),
      skill1: weapon.skill1.level.took_skill ? new WeaponSkill(weapon.skill1) : null,
      skill2: weapon.skill2.level.took_skill ? new WeaponSkill(weapon.skill2) : null,
    };
  }

  get id () {
    return this._.id;
  }

  get name () {
    return this._.name;
  }

  get kind () {
    return this._.kind;
  }

  get attribute () {
    return this._.attribute;
  }

  get attack () {
    return this._.attack;
  }

  get imageURL () {
    return `http://game-a.granbluefantasy.jp/assets/img_mid/sp/assets/weapon/b/${this._.master.id}.png`;
  }

  get skillLevel () {
    return this._.skillLevel;
  }

  get skill1 () {
    return this._.skill1;
  }

  get skill2 () {
    return this._.skill2;
  }

}


class WeaponSkill {

  constructor (skill) {
    this._ = {
      name: skill.name,
      comment: skill.comment,
    };
  }

  get name () {
    return this._.name;
  }

  get comment () {
    return this._.comment;
  }

}
