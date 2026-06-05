const vscode = require('vscode');
const http = require('http');

function activate(context) {
    let rastreador = vscode.window.onDidChangeTextEditorSelection((event) => {
        if (!event.selections || event.selections.length === 0) return;
        const linha = event.selections[0].active.line + 1;
        http.get(`http://localhost:9000/?linha=${linha}`).on('error', (err) => {
        });
    });
    context.subscriptions.push(rastreador);
}

exports.activate = activate;
