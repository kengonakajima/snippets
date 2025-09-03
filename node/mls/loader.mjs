// ESMローダー: ts-mls の相対インポートに .js を補完
export async function resolve(specifier, context, nextResolve) {
  const parent = context.parentURL || "";
  const isRelative = specifier.startsWith("./") || specifier.startsWith("../");
  const hasExt = /\.(mjs|cjs|js|json|node)$/.test(specifier);

  if (
    isRelative &&
    !hasExt &&
    parent.includes("/node_modules/ts-mls/dist/src/")
  ) {
    try {
      return await nextResolve(specifier + ".js", context);
    } catch {}
  }
  return nextResolve(specifier, context);
}

