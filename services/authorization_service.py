import time

from flask import Flask, request, Response

app = Flask(__name__)

AUTHORIZED_USERS = [
    "John",
    "Robert",
    "Katherine"
]

@app.route('/authorize')
def authorization_endpoint():
    # beep beep - pretending to be doing something
    time.sleep(3)

    user = request.headers["user"]
    print(f"user authenticating: {user}")
    if user in AUTHORIZED_USERS:
        return Response("OK", 200)
    return Response("Unauthorized", 401)

if __name__ == '__main__':
    app.run(host="localhost", port=9080)
