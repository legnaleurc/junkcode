import typescript from "@rollup/plugin-typescript";

/** @type {import('rollup').RollupOptions} */
export default {
  input: "./src/main.ts",
  output: {
    dir: "build",
  },
  external: ["twitter-api-sdk", "date-fns", "fs/promises", "dotenv"],
  plugins: [typescript()],
};
