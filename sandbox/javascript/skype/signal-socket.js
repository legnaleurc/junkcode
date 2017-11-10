class SignalSocket {

    constructor () {
        this._socket = new WebSocket('ws://localhost:8000/socket');
        this._id = null;
        this._id_promise = new Promise((resolve) => {
            this._id_resolver = resolve;
        });

        this._socket.addEventListener('message', (event) => {
            let rv = event.data;
            rv = JSON.parse(rv);
            this._onMessage(rv);
        });
    }

    async prepare () {
        return this._id_promise;
    }

    get id () {
        return this._id;
    }

    async _onMessage (data) {
        if (data.action === 'login') {
            this._id_resolver(data.args.id);
            delete this._id_resolver;
            return;
        }

        if (data.action === 'new-member') {
            await this.onnewmember(data.args.id);
            return;
        }

        if (data.action === 'audio-offer') {
            await this.onaudiooffer(data.args.sender, data.args.sdp);
            return;
        }

        if (data.action === 'audio-answer') {
            await this.onaudioanswer(data.args.sdp);
            return;
        }

        if (data.action === 'new-ice-candidate') {
            await this.onnewicecandidate(data.args.sender, data.args.candidate);
            return;
        }
    }

    send (data) {
        data = JSON.stringify(data);
        this._socket.send(data);
    }

}
