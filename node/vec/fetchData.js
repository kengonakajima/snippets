import fetch from 'node-fetch';
import fs from 'fs/promises';
import path from 'path';

async function fetchSampleData() {
    const dataDir = './data';
    
    await fs.mkdir(dataDir, { recursive: true });
    
    const wineDataUrl = 'https://archive.ics.uci.edu/ml/machine-learning-databases/wine-quality/winequality-red.csv';
    const wineDescriptions = [
        {
            id: 'wine_1',
            text: 'This Portuguese red wine has high fixed acidity and medium volatile acidity, with citric acid providing freshness. A dry wine with moderate residual sugar.',
            category: 'Vinho Verde'
        },
        {
            id: 'wine_2',
            text: 'Full-bodied red wine with low acidity and higher alcohol content. Rich tannins and complex flavor profile with notes of dark fruit.',
            category: 'Douro'
        },
        {
            id: 'wine_3',
            text: 'Light and crisp wine with high total sulfur dioxide for preservation. Balanced pH and moderate density make it refreshing.',
            category: 'Minho'
        },
        {
            id: 'wine_4',
            text: 'Robust wine with significant free sulfur dioxide and chlorides. The high sulphates contribute to its aging potential.',
            category: 'Alentejo'
        },
        {
            id: 'wine_5',
            text: 'Medium-bodied wine with balanced fixed and volatile acidity. Pleasant citric notes with moderate alcohol content.',
            category: 'Lisboa'
        },
        {
            id: 'wine_6',
            text: 'Elegant wine with low volatile acidity and refined tannin structure. The pH level indicates good aging capability.',
            category: 'Bairrada'
        },
        {
            id: 'wine_7',
            text: 'Complex red wine featuring high fixed acidity balanced by residual sugars. Dense structure with concentrated flavors.',
            category: 'Dão'
        },
        {
            id: 'wine_8',
            text: 'Fresh and vibrant wine with pronounced citric acid. Low chloride content and balanced sulphates create a clean finish.',
            category: 'Tejo'
        },
        {
            id: 'wine_9',
            text: 'Bold wine with elevated alcohol and low residual sugar. The density and pH suggest a full-bodied character.',
            category: 'Setúbal'
        },
        {
            id: 'wine_10',
            text: 'Delicate wine with moderate volatile acidity and soft tannins. Well-integrated sulfur dioxide preserves fruit character.',
            category: 'Beira Interior'
        }
    ];

    const newsArticles = [
        {
            id: 'news_1',
            text: 'Recent advances in artificial intelligence have revolutionized natural language processing, enabling more sophisticated text understanding and generation capabilities.',
            category: 'Technology'
        },
        {
            id: 'news_2',
            text: 'Climate scientists report unprecedented temperature increases in polar regions, with ice sheet melting accelerating beyond previous projections.',
            category: 'Environment'
        },
        {
            id: 'news_3',
            text: 'Global financial markets experience volatility as central banks adjust interest rates in response to changing inflation expectations.',
            category: 'Finance'
        },
        {
            id: 'news_4',
            text: 'Medical researchers announce breakthrough in gene therapy treatments, offering new hope for patients with rare genetic disorders.',
            category: 'Health'
        },
        {
            id: 'news_5',
            text: 'Space exploration reaches new milestone as private companies successfully deploy satellite constellations for global internet coverage.',
            category: 'Space'
        },
        {
            id: 'news_6',
            text: 'Agricultural innovations using vertical farming and hydroponics promise to address food security challenges in urban environments.',
            category: 'Agriculture'
        },
        {
            id: 'news_7',
            text: 'Quantum computing advances bring practical applications closer to reality, with potential impacts on cryptography and drug discovery.',
            category: 'Technology'
        },
        {
            id: 'news_8',
            text: 'Renewable energy installations surpass fossil fuel capacity for the first time in several major economies, marking historic transition.',
            category: 'Energy'
        },
        {
            id: 'news_9',
            text: 'Educational institutions adopt hybrid learning models, combining traditional classroom instruction with digital platforms and AI tutors.',
            category: 'Education'
        },
        {
            id: 'news_10',
            text: 'Urban planners implement smart city technologies to optimize traffic flow, reduce emissions, and improve quality of life for residents.',
            category: 'Urban Development'
        }
    ];

    const productDescriptions = [
        {
            id: 'product_1',
            text: 'Premium wireless headphones with active noise cancellation, 30-hour battery life, and superior sound quality for audiophiles.',
            category: 'Electronics'
        },
        {
            id: 'product_2',
            text: 'Ergonomic office chair with lumbar support, breathable mesh back, and adjustable armrests for all-day comfort.',
            category: 'Furniture'
        },
        {
            id: 'product_3',
            text: 'High-performance running shoes with responsive cushioning, lightweight design, and durable rubber outsole for marathon training.',
            category: 'Sports'
        },
        {
            id: 'product_4',
            text: 'Stainless steel water bottle with double-wall vacuum insulation, keeping drinks cold for 24 hours or hot for 12 hours.',
            category: 'Lifestyle'
        },
        {
            id: 'product_5',
            text: 'Smart home security camera with 4K resolution, night vision, motion detection, and two-way audio communication.',
            category: 'Home Security'
        },
        {
            id: 'product_6',
            text: 'Professional-grade kitchen knife set with high-carbon stainless steel blades and comfortable non-slip handles.',
            category: 'Kitchen'
        },
        {
            id: 'product_7',
            text: 'Portable solar charger with multiple USB ports, waterproof design, and fast-charging capability for outdoor adventures.',
            category: 'Outdoor'
        },
        {
            id: 'product_8',
            text: 'Luxury skincare set featuring vitamin C serum, retinol cream, and hyaluronic acid moisturizer for radiant skin.',
            category: 'Beauty'
        },
        {
            id: 'product_9',
            text: 'Gaming laptop with latest graphics card, high-refresh display, and advanced cooling system for competitive gaming.',
            category: 'Gaming'
        },
        {
            id: 'product_10',
            text: 'Yoga mat with extra thickness, non-slip surface, and eco-friendly materials for comfortable practice sessions.',
            category: 'Fitness'
        }
    ];

    const allDocuments = [
        ...wineDescriptions,
        ...newsArticles,
        ...productDescriptions
    ];

    await fs.writeFile(
        path.join(dataDir, 'sample_documents.json'),
        JSON.stringify(allDocuments, null, 2)
    );

    console.log(`✅ Saved ${allDocuments.length} sample documents to data/sample_documents.json`);
    console.log(`   - ${wineDescriptions.length} wine descriptions`);
    console.log(`   - ${newsArticles.length} news articles`);
    console.log(`   - ${productDescriptions.length} product descriptions`);
    
    return allDocuments;
}

export default fetchSampleData;

if (import.meta.url === `file://${process.argv[1]}`) {
    fetchSampleData().catch(console.error);
}