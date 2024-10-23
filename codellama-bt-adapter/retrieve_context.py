from sentence_transformers import SentenceTransformer
from pinecone import Pinecone

# Initialize Pinecone with your API key
pc = Pinecone(api_key="lol")
model = SentenceTransformer('all-MiniLM-L6-v2')

def get_text_embedding(text_query):
    # Use Sentence Transformers to get the embedding
    return model.encode(text_query).tolist()  # Convert to list for compatibility

def retrieve_similar_content(query, index_name, top_k=5):
    # Generate query embedding
    query_embedding = get_text_embedding(query)

    # Connect to Pinecone index
    index = pc.Index(index_name)

    # Query Pinecone for similar vectors
    results = index.query(vector=query_embedding, top_k=top_k, include_metadata=True)

    return results

# Example usage
if __name__ == "__main__":
    # Input query text
    print("What behavior tree do you want to generate: ")
    query_text = input()
    
    # Get similar content
    result = retrieve_similar_content(query_text, "pdf-rag-index")

    # Use result in LLM query as context here
