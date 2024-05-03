FROM node:20.12.1-bookworm AS pre-build

RUN : \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

USER node


FROM pre-build AS dev-build

ARG SQLITE_FILE
ARG PUBLIC_API_ORIGIN

WORKDIR /app

COPY . /app
RUN npm ci
RUN npm run build


FROM pre-build AS prd-build

WORKDIR /app

COPY package.json package-lock.json /app/
RUN npm ci --omit dev


FROM node:20.12.1-bookworm AS production

USER node
WORKDIR /app

COPY --chown=node:node --from=dev-build /app/build /app/build
COPY --chown=node:node --from=prd-build /app/node_modules /app/node_modules
COPY --chown=node:node drizzle /app/drizzle
COPY --chown=node:node package.json package-lock.json /app/
