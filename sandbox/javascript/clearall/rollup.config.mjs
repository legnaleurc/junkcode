import typescript from "@rollup/plugin-typescript";

export default () => ({
  input: "./src/main.ts",
  output: {
    dir: "build",
  },
  plugins: [typescript()],
});
