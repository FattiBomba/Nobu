import os

import uvicorn


def main() -> None:
    host = os.getenv("NOBU_HOST", "0.0.0.0")
    port = int(os.getenv("NOBU_PORT", "8765"))
    uvicorn.run("bridge.main:app", host=host, port=port, reload=False)


if __name__ == "__main__":
    main()

