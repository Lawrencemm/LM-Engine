# LM-Engine Design Document
## Human Readable Assets
File formats for assets like maps and archetypes (prefabs) are human-readable
with minimal noise. There are no GUIDs that change with every save of the asset. 
There is only plain meaningful data.

This makes assets first-class citizens in source code management tools like git.
They can be merged and have conflicts resolved like source code.
